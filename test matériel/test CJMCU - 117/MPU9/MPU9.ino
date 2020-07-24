#include <MPU9250_asukiaaa.h>
 
/*#ifdef _ESP32_HAL_I2C_H_
//TTGO-T5
#define SDA_PIN 21 //19
#define SCL_PIN 22 //18
//TTGO-TS
//#define SDA_PIN 19
//#define SCL_PIN 18
#endif*/

#define POWER_PIN 12
#define POWER_PIN_STATE HIGH 

#define VARIO_SDA_PIN 27
#define VARIO_SCL_PIN 32  
 
MPU9250_asukiaaa mySensor;
 
void setup() {
  while(!Serial);

  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, POWER_PIN_STATE);     // turn on POWER

  delay(3000);
  Serial.begin(115200);
  Serial.println("started");
 
/*#ifdef _ESP32_HAL_I2C_H_
// for esp32
//Wire.begin(19, 18); //sda, scl
Wire.begin(VARIO_SDA_PIN, VARIO_SCL_PIN); //SDA_PIN, SCL_PIN); //sda, scl
#else
Wire.begin();
#endif*/

  Wire.begin(VARIO_SDA_PIN, VARIO_SCL_PIN); //SDA_PIN, SCL_PIN); //sda, scl
 
  mySensor.setWire(&Wire);
 
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();
 
// you can set your own offset for mag values
// mySensor.magXOffset = -50;
// mySensor.magYOffset = -55;
// mySensor.magZOffset = -10;
}
 
void loop() {

  uint8_t sensorId;
  if (mySensor.readId(&sensorId) == 0) {
    Serial.println("sensorId: " + String(sensorId));
  } else {
    Serial.println("Cannot read sensorId");
  }
  
  if (mySensor.accelUpdate() == 0) {
    Serial.println("print accel values");
    Serial.println("accelX: " + String(mySensor.accelX()));
    Serial.println("accelY: " + String(mySensor.accelY()));
    Serial.println("accelZ: " + String(mySensor.accelZ()));
    Serial.println("accelSqrt: " + String(mySensor.accelSqrt()));
  } else {
    Serial.println("Cannod read accel values");
  }

  if (mySensor.gyroUpdate() == 0) {
    Serial.println("gyroX: " + String(mySensor.gyroX()));
    Serial.println("gyroY: " + String(mySensor.gyroY()));
    Serial.println("gyroZ: " + String(mySensor.gyroZ()));
  } else {
    Serial.println("Cannot read gyro values");
  }
  
  if (mySensor.magUpdate() == 0) {
    Serial.println("print mag values");
    Serial.println("magX: " + String(mySensor.magX()));
    Serial.println("maxY: " + String(mySensor.magY()));
    Serial.println("magZ: " + String(mySensor.magZ()));
    Serial.println("horizontal direction: " + String(mySensor.magHorizDirection()));
   } else {
    Serial.println("Cannot read mag values");
  }

  Serial.println("at " + String(millis()) + "ms");
  delay(500);
}
