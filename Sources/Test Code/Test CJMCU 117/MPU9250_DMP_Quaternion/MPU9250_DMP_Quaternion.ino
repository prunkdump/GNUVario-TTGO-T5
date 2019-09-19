/************************************************************
MPU9250_DMP_Quaternion
 Quaternion example for MPU-9250 DMP Arduino Library 
Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library

The MPU-9250's digital motion processor (DMP) can calculate
four unit quaternions, which can be used to represent the
rotation of an object.

This exmaple demonstrates how to configure the DMP to 
calculate quaternions, and prints them out to the serial
monitor. It also calculates pitch, roll, and yaw from those
values.

Development environment specifics:
Arduino IDE 1.6.12
SparkFun 9DoF Razor IMU M0

Supported Platforms:
- ATSAMD21 (Arduino Zero, SparkFun SAMD21 Breakouts)
*************************************************************/

// https://learn.sparkfun.com/tutorials/9dof-razor-imu-m0-hookup-guide#using-the-mpu-9250-dmp-arduino-library
// https://www.hackster.io/donowak/esp32-mpu9250-3d-orientation-visualisation-467dc1

//https://create.arduino.cc/projecthub/30503/using-the-mpu9250-to-get-real-time-motion-data-08f011

//https://www.coordonnees-gps.fr/
//https://www.ngdc.noaa.gov/geomag/calculators/magcalc.shtml
// yam =-1.35;

//257  -793  324 -3  13  7

#include <MPU9250-DMP_SF_EXT.h>
#include <MS5611-Ext.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#if !defined(SerialPort)
#define SerialPort Serial
#endif

MPU9250_DMP imu;

MS5611 ms5611;
double referencePressure;
long accelBias[3], gyroBias[3];

//#define SAVE_EEPROM

void setup() 
{
  SerialPort.begin(115200);

  delay(5000);
  
  // Initialize MS5611 sensor
  SerialPort.println("Initialize MS5611 Sensor");

#if defined(VARIO_SDA_PIN) && defined(VARIO_SCL_PIN)
  while(!ms5611.begin(VARIO_SDA_PIN, VARIO_SCL_PIN))
#else
  while(!ms5611.begin())
#endif
  {
    SerialPort.println("Could not find a valid MS5611 sensor, check wiring!");
    delay(500);
  }

  // Get reference pressure for relative altitude
  referencePressure = ms5611.readPressure();

  // Check settings
  checkSettings();
  
  // Call imu.begin() to verify communication and initialize
#if defined(VARIO_SDA_PIN) && defined(VARIO_SCL_PIN)
  if (imu.begin(VARIO_SDA_PIN, VARIO_SCL_PIN) != INV_SUCCESS)
#else
  if (imu.begin() != INV_SUCCESS)
#endif
  {
    while (1)
    {
      SerialPort.println("Unable to communicate with MPU-9250");
      SerialPort.println("Check connections, and try again.");
      SerialPort.println();
      delay(5000);
    }
  }
  
/*  imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT | // Enable 6-axis quat
               DMP_FEATURE_GYRO_CAL, // Use gyro calibration
              10); // Set DMP FIFO rate to 10 Hz
  // DMP_FEATURE_LP_QUAT can also be used. It uses the 
  // accelerometer in low-power mode to estimate quat's.
  // DMP_FEATURE_LP_QUAT and 6X_LP_QUAT are mutually exclusive*/

  imu.setSensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);

/*
  // Use setGyroFSR() and setAccelFSR() to configure the
  // gyroscope and accelerometer full scale ranges.
  // Gyro options are +/- 250, 500, 1000, or 2000 dps
  imu.setGyroFSR(2000); // Set gyro to 2000 dps
  // Accel options are +/- 2, 4, 8, or 16 g
  imu.setAccelFSR(8); // Set accel to +/-2g
  // Note: the MPU-9250's magnetometer FSR is set at 
  // +/- 4912 uT (micro-tesla's)

  // setLPF() can be used to set the digital low-pass filter
  // of the accelerometer and gyroscope.
  // Can be any of the following: 188, 98, 42, 20, 10, 5
  // (values are in Hz).
  imu.setLPF(5); // Set LPF corner frequency to 5Hz*/

  // The sample rate of the accel/gyro can be set using
  // setSampleRate. Acceptable values range from 4Hz to 1kHz
  imu.setSampleRate(100); // Set sample rate to 10Hz

  // Likewise, the compass (magnetometer) sample rate can be
  // set using the setCompassSampleRate() function.
  // This value can range between: 1-100Hz
  imu.setCompassSampleRate(100); // Set mag rate to 10Hz  
  imu.dmpBegin(DMP_FEATURE_SEND_RAW_ACCEL | // Send accelerometer data
 //              DMP_FEATURE_SEND_RAW_GYRO  | // Send raw gyroscope values to FIFO
               DMP_FEATURE_GYRO_CAL       | // Calibrate the gyro data
               DMP_FEATURE_SEND_CAL_GYRO  | // Send calibrated gyro data
               DMP_FEATURE_6X_LP_QUAT     , // Calculate quat's with accel/gyro
               100);                         // Set update rate to 10Hz.

#if defined(SAVE_EEPROM)
  accelBias[0]  = 257;
  accelBias[1]  = -793;
  accelBias[2]  = 324;
  
  gyroBias[3]   = -3;
  gyroBias[3]   = 13;
  gyroBias[3]   = 7;
  
  imu.set_accel_biases(accelBias);  
  imu.set_gyro_biases(gyroBias);
  imu.WriteEeprom();  

#else
  accelBias[0]  = 0;
  accelBias[1]  = 0;
  accelBias[2]  = 0;
  
  gyroBias[3]   = 0;
  gyroBias[3]   = 0;
  gyroBias[3]   = 0;
  
  imu.ReadEeprom();  
  imu.get_accel_biases(accelBias);  
  SerialPort.println("Accel Bias X : " + String(accelBias[0]));
  SerialPort.println("Accel Bias Y : " + String(accelBias[1]));
  SerialPort.println("Accel Bias Z : " + String(accelBias[2]));    

  imu.get_gyro_biases(gyroBias);
  SerialPort.println("Gyro Bias X : " + String(gyroBias[0]));
  SerialPort.println("Gyro Bias Y : " + String(gyroBias[1]));
  SerialPort.println("Gyro Bias Z : " + String(gyroBias[2]));    
#endif  
}

void loop() 
{  
    
  // Check for new data in the FIFO
  if ( imu.fifoAvailable() )
  {

    // Read raw values
    uint32_t rawTemp = ms5611.readRawTemperature();
    uint32_t rawPressure = ms5611.readRawPressure();

    // Read true temperature & Pressure
    double realTemperature = ms5611.readTemperature();
    long realPressure = ms5611.readPressure();

    // Calculate altitude
    float absoluteAltitude = ms5611.getAltitude(realPressure);
    float relativeAltitude = ms5611.getAltitude(realPressure, referencePressure);

    SerialPort.print("rawTemp = ");
    SerialPort.print(rawTemp);
    SerialPort.print(", realTemp = ");
    SerialPort.print(realTemperature);
    SerialPort.println(" *C");

    SerialPort.print("rawPressure = ");
    SerialPort.print(rawPressure);
    SerialPort.print(", realPressure = ");
    SerialPort.print(realPressure);
    SerialPort.println(" Pa");

    SerialPort.print("absoluteAltitude = ");
    SerialPort.print(absoluteAltitude);
    SerialPort.print(" m, relativeAltitude = ");
    SerialPort.print(relativeAltitude);    
    SerialPort.println(" m");
    SerialPort.println();


    if ( imu.dataReady() )
    {
      // Call update() to update the imu objects sensor data.
      // You can specify which sensors to update by combining
      // UPDATE_ACCEL, UPDATE_GYRO, UPDATE_COMPASS, and/or
      // UPDATE_TEMPERATURE.
      // (The update function defaults to accel, gyro, compass,
      //  so you don't have to specify these values.)
      imu.updateCal(UPDATE_TEMP | UPDATE_COMPASS);
      printIMUData3();
    }
       
    // Use dmpUpdateFifo to update the ax, gx, mx, etc. values
    if ( imu.dmpUpdateFifo() == INV_SUCCESS)
    {
      // computeEulerAngles can be used -- after updating the
      // quaternion values -- to estimate roll, pitch, and yaw
      imu.computeEulerAngles();
      printIMUData();
      printIMUData2();
    }
  }
}

void printIMUData(void)
{  
  // After calling dmpUpdateFifo() the ax, gx, mx, etc. values
  // are all updated.
  // Quaternion values are, by default, stored in Q30 long
  // format. calcQuat turns them into a float between -1 and 1
  float q0 = imu.calcQuat(imu.qw);
  float q1 = imu.calcQuat(imu.qx);
  float q2 = imu.calcQuat(imu.qy);
  float q3 = imu.calcQuat(imu.qz);

  SerialPort.println("Q: " + String(q0, 4) + ", " +
                    String(q1, 4) + ", " + String(q2, 4) + 
                    ", " + String(q3, 4));
  SerialPort.print("R/P/Y: " + String(imu.roll) + ", "
            + String(imu.pitch) + ", " + String(imu.yaw-1.35));

  float tmpyaw = imu.yaw-1.35-25;

 SerialPort.print(" / Yaw: " + String(tmpyaw));
  
  if (tmpyaw <  0) tmpyaw += 360;
  if (tmpyaw >  360) tmpyaw -= 360;
  if ((tmpyaw > 337.5 && tmpyaw <= 360) || (tmpyaw > 0 &&  tmpyaw < 22.5))  SerialPort.println(" - N ");
  else if (tmpyaw > 22.5 && tmpyaw <= 67.5) SerialPort.println(" - NE ");
  else if (tmpyaw > 67.5 && tmpyaw <= 112.5) SerialPort.println(" - E ");
  else if (tmpyaw > 112.5 && tmpyaw <= 157.5) SerialPort.println(" - SE ");
  else if (tmpyaw > 157.5 && tmpyaw <= 202.5) SerialPort.println(" - S ");
  else if (tmpyaw > 202.5 && tmpyaw <= 247.5) SerialPort.println(" - SO ");
  else if (tmpyaw > 247.5 && tmpyaw <= 292.5) SerialPort.println(" - O ");
  else if (tmpyaw > 292.5 && tmpyaw <= 337.5) SerialPort.println(" - NO ");

//  SerialPort.println("Time: " + String(imu.time) + " ms");
  SerialPort.println();
}

void printIMUData2(void)
{  
  // After calling update() the ax, ay, az, gx, gy, gz, mx,
  // my, mz, time, and/or temerature class variables are all
  // updated. Access them by placing the object. in front:

  // Use the calcAccel, calcGyro, and calcMag functions to
  // convert the raw sensor readings (signed 16-bit values)
  // to their respective units.
  float accelX = imu.calcAccel(imu.ax);
  float accelY = imu.calcAccel(imu.ay);
  float accelZ = imu.calcAccel(imu.az);
  float gyroX = imu.calcGyro(imu.gx);
  float gyroY = imu.calcGyro(imu.gy);
  float gyroZ = imu.calcGyro(imu.gz);
  
  SerialPort.println("Accel: " + String(accelX) + ", " +
              String(accelY) + ", " + String(accelZ) + " g");
  SerialPort.println("Gyro: " + String(gyroX) + ", " +
              String(gyroY) + ", " + String(gyroZ) + " dps");
  SerialPort.println("Time: " + String(imu.time) + " ms");
  SerialPort.println();
}

void printIMUData3(void)
{  
  // After calling update() the ax, ay, az, gx, gy, gz, mx,
  // my, mz, time, and/or temerature class variables are all
  // updated. Access them by placing the object. in front:

  // Use the calcAccel, calcGyro, and calcMag functions to
  // convert the raw sensor readings (signed 16-bit values)
  // to their respective units.
  float magX = imu.calcMag(imu.mx);
  float magY = imu.calcMag(imu.my);
  float magZ = imu.calcMag(imu.mz);
  
  SerialPort.println("Mag: " + String(magX) + ", " +
              String(magY) + ", " + String(magZ) + " uT");
 
  // Calculate the angle of the vector y,x
  float heading = imu.computeCompassHeading();
 
//  SerialPort.println("Compass Heading: " + String(heading) + " Deg");
              
//  SerialPort.println("Time: " + String(imu.time) + " ms");
  SerialPort.println();
}

/*
   // After calling update() the ax, ay, az, gx, gy, gz, mx,
  // my, mz, time, and/or temerature class variables are all
  // updated. Access them by placing the object. in front:

  // Use the calcAccel, calcGyro, and calcMag functions to
  // convert the raw sensor readings (signed 16-bit values)
  // to their respective units.
  float accelX = imu.calcAccel(imu.ax);
  float accelY = imu.calcAccel(imu.ay);
  float accelZ = imu.calcAccel(imu.az);
  float gyroX = imu.calcGyro(imu.gx);
  float gyroY = imu.calcGyro(imu.gy);
  float gyroZ = imu.calcGyro(imu.gz);
  float magX = imu.calcMag(imu.mx);
  float magY = imu.calcMag(imu.my);
  float magZ = imu.calcMag(imu.mz);
  
  SerialPort.println("Accel: " + String(accelX) + ", " +
              String(accelY) + ", " + String(accelZ) + " g");
  SerialPort.println("Gyro: " + String(gyroX) + ", " +
              String(gyroY) + ", " + String(gyroZ) + " dps");
  SerialPort.println("Mag: " + String(magX) + ", " +
              String(magY) + ", " + String(magZ) + " uT");
  SerialPort.println("Time: " + String(imu.time) + " ms");
  SerialPort.println();
 */
 
void checkSettings()
{
  SerialPort.print("Oversampling: ");
  SerialPort.println(ms5611.getOversampling());
}
