// ESP32 I2C Scanner
// vient de https://www.esp32.com/viewtopic.php?t=4742
// ajout de la spécificité du GNUVario-E, pour déclencher l'alimentation électrique du GPS et du CJMCU 

#include <Wire.h>

#define SDA_PIN 27
#define SCL_PIN 32
#define POWER_PIN 12
#define POWER_PIN_STATE HIGH 

void setup()
{
  Serial.begin (115200);
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, POWER_PIN_STATE);     // turn on POWER
  delay(5000);
  Wire.begin (SDA_PIN, SCL_PIN);
}

void Scanner ()
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);            // Begin I2C transmission Address (i)
    byte error = Wire.endTransmission ();
    if (error == 0)                        // Receive 0 = success (ACK response) 
    {
      Serial.print ("I2C device found at address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
    }
    else    // 1 = data too long to fit in transmit buffer 
            // 2 = received NACK on transmit of address
            // 3 = received NACK on transmit of data
            // 4 = other error
    {
      if (error==4)
      {
        Serial.print("#### Unknow error at address: ");
        Serial.print (i, DEC);
        Serial.print (" (0x");
        Serial.print (i, HEX);
        Serial.println (") ####");
      }
    }
  }
  Serial.print ("Found ");      
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s).");
}

void loop()
{
  Scanner ();
  delay (1000);
}
