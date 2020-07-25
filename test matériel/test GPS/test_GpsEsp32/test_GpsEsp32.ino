
#include <Arduino.h>
#include <HardwareConfig.h>

#include <DebugConfig.h>

#ifdef DATA_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif

#define ARDUINOTRACE_SERIAL SerialPort
#include <ArduinoTrace.h>

#include <TinyGPS++.h>

char data;
double latitude;
double longitude;
double alt; //altitude
double vitesse;
unsigned long nbre_sat;

uint8_t heures;
uint8_t minutes;
uint8_t secondes;

 
TinyGPSPlus gps;
HardwareSerial SerialGPS(2);
 
void setup() {

  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, POWER_PIN_STATE);     // turn on POWER

  delay(5000);
  
  //Initialize serial and wait for port to open:
  //Serial1.begin(9600);
  SerialGPS.begin(9600, SERIAL_8N1, pinGpsRXD, pinGpsTXD, false);  //-1
  Serial.begin(115000);
  SerialPort.println("Begin");
  while (!SerialPort) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

 // for (int i=0; i< 100; i++) SerialGPS.write("A");
   SerialPort.println("Loop");

}
 
void loop() {
  while (SerialGPS.available()) {
    data = SerialGPS.read();
    SerialPort.print(data);
    gps.encode(data);
    if (gps.location.isUpdated())
    {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      alt = gps.altitude.meters();
      vitesse = gps.speed.kmph();
      nbre_sat = gps.satellites.value();

      SerialPort.println("");
      SerialPort.println("-------- FIX GPS ------------");
      SerialPort.print("LATITUDE="); SerialPort.println(latitude);
      SerialPort.print("LONGITUDE="); SerialPort.println(longitude);
      SerialPort.print("ALTITUDE (m) ="); SerialPort.println(alt);
      SerialPort.print("VITESSE (km/h)="); SerialPort.println(vitesse);
      SerialPort.print("NOMBRE SATTELLITES="); SerialPort.println(nbre_sat);
      SerialPort.flush();
    }

    if (gps.time.isUpdated())
    {
      heures   = gps.time.hour();
      minutes  = gps.time.minute();
      secondes = gps.time.second();

      SerialPort.println("");
      SerialPort.println("-------- TIME GPS ------------");
      SerialPort.print(heures);
      SerialPort.print(":"); 
      SerialPort.print(minutes);
      SerialPort.print(":"); 
      SerialPort.println(secondes);
      SerialPort.flush();
    }    
  }
}

/*
 * $GNGLL,4546.2255,N,00311.6571,E,053151.000,A,A*41
$GPGSA,A,3,02,06,30,23,05,09,13,07,,,,,1.8,1.2,1.4*32

-------- FIX GPS ------------
LATITUDE=0.05
LONGITUDE=0.10
ALTITUDE (m) =13.00
VITESSE (km/h)=0.00
NOMBRE SATTELLITES=5

-------- TIME GPS ------------
0:0:0

$BDGSA,A,3,,,,,,,,,,,,,1.8,1.2,1.4*2C
$GPGSV,3,1,10,02,37,246,44,04,,,35,05,48,304,31,06,23,202,43*42
$GPGSV,3,2,10,07,64,074,41,09,39,074,28,13,18,263,44,16,07,028,*76
$GPGSV,3,3,10,23,08,081,26,30,66,173,40*7E
$BDGSV,1,1,00*68
$GNRMC,053151.000,A,4546.2255,N,00311.6571,E,0.00,0.00,020419,,,A*78

-------- FIX GPS ------------
LATITUDE=45.77
LONGITUDE=3.19
ALTITUDE (m) =13.00
VITESSE (km/h)=0.00
NOMBRE SATTELLITES=5
*/
