#include <Arduino.h>
#include <HardwareConfig.h>
#include <VarioSettings.h>
#include <DebugConfig.h>
#include <IntTW.h>
#include <ms5611.h>
#include <vertaccel.h>
#include <LightInvensense.h>
#include <TwoWireScheduler.h>

#include <kalmanvert.h>

#include <toneHAL.h>
#include <beeper.h>

#include <SerialNmea.h>
#include <NmeaParser.h>
#include <LxnavSentence.h>
#include <LK8Sentence.h>
#include <IGCSentence.h>
#include <FlightHistory.h>

#include <sdcardHAL.h>

/*******************/
/* General objects */
/*******************/
#define VARIOMETER_STATE_INITIAL 0
#define VARIOMETER_STATE_DATE_RECORDED 1
#define VARIOMETER_STATE_CALIBRATED 2
#define VARIOMETER_STATE_FLIGHT_STARTED 3

#ifdef HAVE_GPS
uint8_t variometerState = VARIOMETER_STATE_INITIAL;
#else
uint8_t variometerState = VARIOMETER_STATE_CALIBRATED;
#endif //HAVE_GPS

/***************/
/* IMU objects */
/***************/
#ifdef HAVE_BMP280
Bmp280 TWScheduler::bmp280;
#else
Ms5611 TWScheduler::ms5611;
#endif
#ifdef HAVE_ACCELEROMETER
Vertaccel TWScheduler::vertaccel;
#endif //HAVE_ACCELEROMETER

//Vertaccel vertaccel;

/**********************/
/* alti/vario objects */
/**********************/
#define POSITION_MEASURE_STANDARD_DEVIATION 0.1
#ifdef HAVE_ACCELEROMETER 
#define ACCELERATION_MEASURE_STANDARD_DEVIATION 0.3
#else
#define ACCELERATION_MEASURE_STANDARD_DEVIATION 0.6
#endif //HAVE_ACCELEROMETER 

kalmanvert kalmanvert;

#ifdef HAVE_SPEAKER

#define volumeDefault 8

#include <beeper.h>

Beeper beeper(volumeDefault);

//#define BEEP_FREQ 800
#endif

/************************************/
/* glide ratio / average climb rate */
/************************************/
#if defined(HAVE_GPS) || defined(VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)

/* determine history params */
#ifdef HAVE_GPS
#ifdef VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE
/* unsure period divide GPS_PERIOD */
constexpr double historyGPSPeriodCountF = (double)(VARIOMETER_INTEGRATED_CLIMB_RATE_DISPLAY_FREQ)*(double)(GPS_PERIOD)/(1000.0);
constexpr int8_t historyGPSPeriodCount = (int8_t)(0.5 + historyGPSPeriodCountF);

constexpr double historyPeriodF = (double)(GPS_PERIOD)/(double)(historyGPSPeriodCount);
constexpr unsigned historyPeriod = (unsigned)(0.5 + historyPeriodF);
#else
/* GPS give the period */
constexpr int8_t historyGPSPeriodCount = 1;
constexpr unsigned historyPeriod = GPS_PERIOD;
#endif //VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE

constexpr double historyCountF = (double)(VARIOMETER_GLIDE_RATIO_INTEGRATION_TIME)/(double)historyPeriod;
constexpr int8_t historyCount = (int8_t)(0.5 + historyCountF);
#else //!HAVE_GPS
constexpr double historyCountF = (double)(VARIOMETER_INTEGRATED_CLIMB_RATE_DISPLAY_FREQ)*(double)(VARIOMETER_CLIMB_RATE_INTEGRATION_TIME)/(1000.0);
constexpr int8_t historyCount = (int8_t)(0.5 + historyCountF);

constexpr double historyPeriodF = (double)(VARIOMETER_CLIMB_RATE_INTEGRATION_TIME)/(double)historyCount;
constexpr unsigned historyPeriod = (unsigned)(0.5 + historyPeriodF);
#endif //HAVE_GPS

/* create history */
#ifdef HAVE_GPS
SpeedFlightHistory<historyPeriod, historyCount, historyGPSPeriodCount> history;
#else
#ifdef VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE
FlightHistory<historyPeriod, historyCount> history;
#endif //VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE
#endif //HAVE_GPS

/* compute climb rate period count when differ from glide ratio period count */
#if defined(HAVE_GPS) && defined(VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)
#if VARIOMETER_CLIMB_RATE_INTEGRATION_TIME > VARIOMETER_GLIDE_RATIO_INTEGRATION_TIME
#error VARIOMETER_CLIMB_RATE_INTEGRATION_TIME must be less or equal than VARIOMETER_GLIDE_RATIO_INTEGRATION_TIME
#endif
constexpr double historyClimbRatePeriodCountF = (double)(VARIOMETER_CLIMB_RATE_INTEGRATION_TIME)/(double)historyPeriod;
constexpr int8_t historyClimbRatePeriodCount = (int8_t)historyClimbRatePeriodCountF;
#endif

#endif //defined(HAVE_GPS) || defined(VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)

/***************/
/* gps objects */
/***************/
#ifdef HAVE_GPS

NmeaParser nmeaParser;

#ifdef HAVE_BLUETOOTH
boolean lastSentence = false;
#endif //HAVE_BLUETOOTH

VarioSettings GnuSettings;

#ifdef HAVE_SDCARD
File fileIgc;
IGCHeader header;
IGCSentence igc;

#define SDCARD_STATE_INITIAL 0
#define SDCARD_STATE_INITIALIZED 1
#define SDCARD_STATE_READY 2
#define SDCARD_STATE_ERROR -1
int8_t sdcardState = SDCARD_STATE_INITIAL;

#endif //HAVE_SDCARD

#endif //HAVE_GPS

/*********************/
/* bluetooth objects */
/*********************/
#ifdef HAVE_BLUETOOTH
#if defined(VARIOMETER_SENT_LXNAV_SENTENCE)
LxnavSentence bluetoothNMEA;
#elif defined(VARIOMETER_SENT_LK8000_SENTENCE)
LK8Sentence bluetoothNMEA;
#else
#error No bluetooth sentence type specified !
#endif

#ifndef HAVE_GPS
unsigned long lastVarioSentenceTimestamp = 0;
#endif // !HAVE_GPS
#endif //HAVE_BLUETOOTH

unsigned long lastDisplayTimestamp;
unsigned long count = 0;
double alti;

/*******************************/
/*******************************/
void setup() {
/*******************************/
/*******************************/  
  
#ifdef PROG_DEBUG
  // Open serial communications and wait for port to open:
  SerialPort.begin(115200);
//  while (!SerialPort) {;}
#endif //PRO_DEBBUG

  /*****************************/
  /* wait for devices power on */
  /*****************************/
  delay(VARIOMETER_POWER_ON_DELAY);

  /******************/
  /* Init Speaker   */
  /******************/
  
#if defined( HAVE_SPEAKER)
  toneHAL.init();
#endif

  /****************/
  /* init SD Card */
  /****************/

#ifdef PROG_DEBUG
  SerialPort.println("Initializing SD card...");
#endif //PRO_DEBBUG

  if (GnuSettings.initSettings()) {
#ifdef PROG_DEBUG
   SerialPort.println("initialization done.");
#endif //PROG_DEBUG

   GnuSettings.readSDSettings();
 
#ifdef PROG_DEBUG
   //Debuuging Printing
 SerialPort.print("Pilot Name = ");
 SerialPort.println(GnuSettings.VARIOMETER_PILOT_NAME);
#endif //PROG_DEBUG
  }
  else
  {
#ifdef HAVE_SPEAKER
    if (GnuSettings.ALARM_SDCARD) {
#ifdef PROG_DEBUG
      SerialPort.println("initialization failed!");
#endif //PROG_DEBUG

//      indicateFaultSDCARD();
    }
#endif //HAVE_SPEAKER 
  }  

  /**************************/
  /* init Two Wires devices */
  /**************************/
  //!!!
  intTW.begin();
  twScheduler.init();
//  vertaccel.init();

  /**************************/
  /* init gps and bluetooth */
  /**************************/
#if defined(HAVE_BLUETOOTH) || defined(HAVE_GPS)
#ifdef HAVE_GPS
  serialNmea.begin(GPS_BLUETOOTH_BAUDS, true);
#else
  serialNmea.begin(GPS_BLUETOOTH_BAUDS, false);
#endif //HAVE_GPS
#endif //defined(HAVE_BLUETOOTH) || defined(HAVE_GPS)

  /******************/
  /* get first data */
  /******************/
  
  /* wait for first alti and acceleration */
  while( ! twScheduler.havePressure() ) { }

#ifdef PROG_DEBUG
    SerialPort.println("première mesure");
#endif //PROG_DEBUG

  /* init kalman filter with 0.0 accel*/
  double firstAlti = twScheduler.getAlti();
#ifdef PROG_DEBUG
    SerialPort.print("firstAlti : ");
    SerialPort.println(firstAlti);
#endif //PROG_DEBUG

  kalmanvert.init(firstAlti,
                  0.0,
                  POSITION_MEASURE_STANDARD_DEVIATION,
                  ACCELERATION_MEASURE_STANDARD_DEVIATION,
                  millis());

#ifdef PROG_DEBUG
    SerialPort.println("kalman init");
#endif //PROG_DEBUG

#if defined(HAVE_GPS) || defined(VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)
  history.init(firstAlti, millis());
#endif //defined(HAVE_GPS) || defined(VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)

  /* init time */
  lastDisplayTimestamp = millis();   
}

/*int16_t accel[3];
int32_t quat[4];*/

#if defined(HAVE_SDCARD) && defined(HAVE_GPS)
void createSDCardTrackFile(void);
#endif //defined(HAVE_SDCARD) && defined(HAVE_GPS)
void enableflightStartComponents(void);

void loop() {

 /* if( vertaccel.readRawAccel(accel, quat) ){
    count++;
  }*/

#ifdef HAVE_ACCELEROMETER
  if( twScheduler.havePressure() && twScheduler.haveAccel() ) {
    
#ifdef PROG_DEBUG
//    SerialPort.println("havePressure && haveAccel");
#endif //PROG_DEBUG

    kalmanvert.update( twScheduler.getAlti(),
                       twScheduler.getAccel(NULL),
                       millis() );
#else
  if( twScheduler.havePressure() ) {
    
#ifdef PROG_DEBUG
//    SerialPort.println("havePressure");
#endif //PROG_DEBUG

    kalmanvert.update( twScheduler.getAlti(),
                       0.0,
                       millis() );
#endif //HAVE_ACCELEROMETER
  }

#ifdef HAVE_SPEAKER
    beeper.setVelocity( kalmanvert.getVelocity() );
#endif //HAVE_SPEAKER

    /* set history */
#if defined(HAVE_GPS) || defined(VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)
    history.setAlti(kalmanvert.getCalibratedPosition(), millis());
#endif
    
//*****************
//* update beeper *
//*****************

#ifdef HAVE_SPEAKER
  beeper.update();
#ifdef PROG_DEBUG
//    SerialPort.println("beeper update");
#endif //PROG_DEBUG

#endif //HAVE_SPEAKER

  /**************/
  /* update GPS */
  /**************/
#ifdef HAVE_GPS
#ifdef HAVE_BLUETOOTH
  /* else try to parse GPS nmea */
  else {
#endif //HAVE_BLUETOOTH
    
    /* try to lock sentences */
    if( serialNmea.lockRMC() ) {
      
#ifdef GPS_DEBUG
    SerialPort.println("mneaParser : beginRMC");
#endif //PROG_DEBUG

      nmeaParser.beginRMC();      
    } else if( serialNmea.lockGGA() ) {
      
#ifdef GPS_DEBUG
    SerialPort.println("mneaParser : beginGGA");
#endif //PROG_DEBUG

      nmeaParser.beginGGA();
#ifdef HAVE_BLUETOOTH
      lastSentence = true;
#endif //HAVE_BLUETOOTH
#ifdef HAVE_SDCARD      
      /* start to write IGC B frames */
      if( sdcardState == SDCARD_STATE_READY ) {
#ifdef VARIOMETER_SDCARD_SEND_CALIBRATED_ALTITUDE
        fileIgc.write( igc.begin( kalmanvert.getCalibratedPosition() ) );
#else
        fileIgc.write( igc.begin( kalmanvert.getPosition() ) );
#endif
      }
#endif //HAVE_SDCARD
    }
  
    /* parse if needed */
    if( nmeaParser.isParsing() ) {

#ifdef GPS_DEBUG
    SerialPort.println("mneaParser : isParsing");
#endif //PROG_DEBUG
      
      while( nmeaParser.isParsing() ) {
        uint8_t c = serialNmea.read();
        
        /* parse sentence */        
        nmeaParser.feed( c );

#ifdef HAVE_SDCARD          
        /* if GGA, convert to IGC and write to sdcard */
        if( sdcardState == SDCARD_STATE_READY && nmeaParser.isParsingGGA() ) {
          igc.feed(c);
          while( igc.available() ) {
            fileIgc.write( igc.get() );
          }
        }
#endif //HAVE_SDCARD
      }
      serialNmea.release();
    
#ifdef HAVE_BLUETOOTH   
      /* if this is the last GPS sentence */
      /* we can send our sentences */
      if( lastSentence ) {
          lastSentence = false;
#ifdef VARIOMETER_BLUETOOTH_SEND_CALIBRATED_ALTITUDE
          bluetoothNMEA.begin(kalmanvert.getCalibratedPosition(), kalmanvert.getVelocity());
#else
          bluetoothNMEA.begin(kalmanvert.getPosition(), kalmanvert.getVelocity());
#endif
          serialNmea.lock(); //will be writed at next loop
      }
#endif //HAVE_BLUETOOTH
    }
    /***************************/
    /* update variometer state */
    /*    (after parsing)      */
    /***************************/
    if( variometerState < VARIOMETER_STATE_FLIGHT_STARTED ) {

      /* if initial state check if date is recorded  */
      if( variometerState == VARIOMETER_STATE_INITIAL ) {
        if( nmeaParser.haveDate() ) {
          variometerState = VARIOMETER_STATE_DATE_RECORDED;
        }
      }
      
      /* check if we need to calibrate the altimeter */
      else if( variometerState == VARIOMETER_STATE_DATE_RECORDED ) {
        
        /* we need a good quality value */
        if( nmeaParser.haveNewAltiValue() && nmeaParser.precision < VARIOMETER_GPS_ALTI_CALIBRATION_PRECISION_THRESHOLD ) {
          
          /* calibrate */
 #ifdef HAVE_SPEAKER 
          if (GnuSettings.ALARM_GPSFIX) {
 //         toneAC(BEEP_FREQ);
            beeper.generateTone(GnuSettings.BEEP_FREQ, 200);
//          delay(200);
//          toneAC(0);
          }
 #endif //defined(HAVE_SPEAKER) 

          double gpsAlti = nmeaParser.getAlti();
          kalmanvert.calibratePosition(gpsAlti);
#if defined(HAVE_GPS) || defined(VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)
          history.init(gpsAlti, millis());
#endif //defined(HAVE_GPS) || defined(VARIOMETER_DISPLAY_INTEGRATED_CLIMB_RATE)

          variometerState = VARIOMETER_STATE_CALIBRATED;

#ifdef HAVE_SDCARD 
          if (!GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START) {
             createSDCardTrackFile();
          }
#endif //HAVE_SDCARD
        }
      }
      
      /* else check if the flight have started */
      else {  //variometerState == VARIOMETER_STATE_CALIBRATED
        
        /* check flight start condition */
        if( (millis() > GnuSettings.FLIGHT_START_MIN_TIMESTAMP) &&
            ((GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START) &&   
             (kalmanvert.getVelocity() < GnuSettings.FLIGHT_START_VARIO_LOW_THRESHOLD || kalmanvert.getVelocity() > GnuSettings.FLIGHT_START_VARIO_HIGH_THRESHOLD)) ||
             (!GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START)
     
  //        && (kalmanvert.getVelocity() < FLIGHT_START_VARIO_LOW_THRESHOLD || kalmanvert.getVelocity() > FLIGHT_START_VARIO_HIGH_THRESHOLD) &&
#ifdef HAVE_GPS
            &&(nmeaParser.getSpeed() > GnuSettings.FLIGHT_START_MIN_SPEED) 
#endif //HAVE_GPS
          ) {
          variometerState = VARIOMETER_STATE_FLIGHT_STARTED;
          enableflightStartComponents();
        }
      }
    }
#ifdef HAVE_BLUETOOTH
  }
#endif //HAVE_BLUETOOTH
#endif //HAVE_GPS

  if( millis() - lastDisplayTimestamp > 1000 ) {

    lastDisplayTimestamp = millis();
    //Serial.println(intTW.lastTwError);
//    SerialPort.println(accel[2]);
    
#ifdef PROG_DEBUG
//    SerialPort.println("loop");

    SerialPort.print("Vario : ");
    SerialPort.println(kalmanvert.getVelocity());

#ifdef HAVE_GPS    
    SerialPort.print("Time : ");
    SerialPort.println(nmeaParser.time);

    uint32_t TmpnewTime = nmeaParser.time;
    uint8_t Tmptime[3];
    if (TmpnewTime > 999999) TmpnewTime = TmpnewTime / 10;
    for( uint8_t i = 0; i<3; i++) {
      Tmptime[i] = (int8_t)(TmpnewTime % 100);
      TmpnewTime /= 100;
    }
  
#ifdef SCREEN_DEBUG
    SerialPort.print("Time : ");
    SerialPort.print(Tmptime[2]);
    SerialPort.print(":");
    SerialPort.print(Tmptime[1]);
    SerialPort.print(":");
    SerialPort.println(Tmptime[0]);
#endif //SCREEN_DEBUG
    

    SerialPort.print("Sat : ");
    SerialPort.println(nmeaParser.satelliteCount);
    
		fileIgc.print(nmeaParser.time);
		fileIgc.print(" - ");
    fileIgc.print("Time : ");
    fileIgc.print(Tmptime[2]);
    fileIgc.print(":");
    fileIgc.print(Tmptime[1]);
    fileIgc.print(":");
    fileIgc.print(Tmptime[0]);
    fileIgc.print(" - ");
		fileIgc.println(kalmanvert.getVelocity());
#endif //HAVE_GPS

#endif //PROG_DEBUG

  }
}

#if defined(HAVE_SDCARD) && defined(HAVE_GPS)
void createSDCardTrackFile(void) {
  /* start the sdcard record */
  if( sdcardState == SDCARD_STATE_INITIALIZED ) {

    /* build date : convert from DDMMYY to YYMMDD */
    uint8_t dateChar[8]; //two bytes are used for incrementing number on filename
    uint8_t* dateCharP = dateChar;
    uint32_t date = nmeaParser.date;
    for(uint8_t i=0; i<3; i++) {
      uint8_t num = ((uint8_t)(date%100));
      dateCharP[0] = (num/10) + '0';
      dateCharP[1] = (num%10) + '0';
      dateCharP += 2;
      date /= 100;
    }

    /* create file */    
    fileIgc = SD.open((char*)dateChar, FILE_WRITE);
    if (fileIgc) {
      sdcardState = SDCARD_STATE_READY;
            
      /* write the header */
      int16_t datePos = header.begin();
      if( datePos >= 0 ) {
        while( datePos ) {
        fileIgc.write(header.get());
          datePos--;
        }

        /* write date : DDMMYY */
        uint8_t* dateCharP = &dateChar[4];
        for(int i=0; i<3; i++) {
          fileIgc.write(dateCharP[0]);
          fileIgc.write(dateCharP[1]);
          header.get();
          header.get();
          dateCharP -= 2;
        }
            
        while( header.available() ) {
          fileIgc.write(header.get());
        }
      }
    } else {
      sdcardState = SDCARD_STATE_ERROR; //avoid retry 
    }
  }
}
#endif //defined(HAVE_SDCARD) && defined(HAVE_GPS)



void enableflightStartComponents(void) {
  
#ifdef HAVE_SPEAKER
if (GnuSettings.ALARM_FLYBEGIN) {
  for( int i = 0; i<2; i++) {
  //   toneAC(BEEP_FREQ);
 //    delay(200);
  //   toneAC(0);
     beeper.generateTone(GnuSettings.BEEP_FREQ, 200);
     delay(200);
  }
}
#endif //HAVE_SPEAKER 

  /* enable near climbing */
#ifdef HAVE_SPEAKER
//#ifdef VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM
if (GnuSettings.VARIOMETER_ENABLE_NEAR_CLIMBING_ALARM) {
  beeper.setGlidingAlarmState(true);
}
//#endif

//#ifdef VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP
if (GnuSettings.VARIOMETER_ENABLE_NEAR_CLIMBING_BEEP) {
  beeper.setGlidingBeepState(true);
}
//#endif
#endif //HAVE_SPEAKER

#if defined(HAVE_SDCARD) && defined(HAVE_GPS) 
//&& defined(VARIOMETER_RECORD_WHEN_FLIGHT_START)
if (GnuSettings.VARIOMETER_RECORD_WHEN_FLIGHT_START) {
  createSDCardTrackFile();
}
#endif // defined(HAVE_SDCARD) && defined(VARIOMETER_RECORD_WHEN_FLIGHT_START)
}
