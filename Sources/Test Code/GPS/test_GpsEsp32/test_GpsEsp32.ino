
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
  //Initialize serial and wait for port to open:
  //Serial1.begin(9600);
  SerialGPS.begin(9600, SERIAL_8N1, 19, -1, false);  //-1
  Serial.begin(115000);
  Serial.println("Begin");
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

 // for (int i=0; i< 100; i++) SerialGPS.write("A");
   Serial.println("Loop");

}
 
void loop() {
  while (SerialGPS.available()) {
    data = SerialGPS.read();
    Serial.print(data);
    gps.encode(data);
    if (gps.location.isUpdated())
    {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      alt = gps.altitude.meters();
      vitesse = gps.speed.kmph();
      nbre_sat = gps.satellites.value();

      Serial.println("");
      Serial.println("-------- FIX GPS ------------");
      Serial.print("LATITUDE="); Serial.println(latitude);
      Serial.print("LONGITUDE="); Serial.println(longitude);
      Serial.print("ALTITUDE (m) ="); Serial.println(alt);
      Serial.print("VITESSE (km/h)="); Serial.println(vitesse);
      Serial.print("NOMBRE SATTELLITES="); Serial.println(nbre_sat);
      Serial.flush();
    }

    if (gps.time.isUpdated())
    {
      heures   = gps.time.hour();
      minutes  = gps.time.minute();
      secondes = gps.time.second();

      Serial.println("");
      Serial.println("-------- TIME GPS ------------");
      Serial.print(heures);
      Serial.print(":"); 
      Serial.print(minutes);
      Serial.print(":"); 
      Serial.println(secondes);
      Serial.flush();
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
