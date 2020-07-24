#include <HGTReader.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

HGTReader hgtReader("/HGT/");

void setup() {
  Serial.begin(115200);
  Serial.println("Setup");
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  truc();

  trucBis();

  truc2();
}

int getGroundLevel(String lat, String lon)
{
  return hgtReader.getGroundLevel(lat, lon);
}
int getGroundLevel(float lat, float lon)
{
  return hgtReader.getGroundLevel(lat, lon);
}
void truc()
{
    unsigned long time1 = millis();

    //Dome du gouter 4292
  // http://api.geonames.org/srtm1?lat=45.842779&lng=6.843396&username=jeromev

  Serial.println(getGroundLevel("45.842779 N", "6.843396 E"));

  // Roc'h Ruz 361
  // http://api.geonames.org/srtm1?lat=48.413305&lng=-3.889676&username=jeromev

   Serial.println(getGroundLevel("48.413305 N", "3.889676 W"));
//
//   // PDD 1411
//   //http://api.geonames.org/srtm1?lat=45.772282&lng=2.961406&username=jeromev
//   Serial.println(getGroundLevel("45.772282 N", "2.961406 E"));
//
//   //2916
//   //http://api.geonames.org/srtm1?lat=42.591652&lng=1.443565&username=jeromev
//   Serial.println(getGroundLevel("42.591652 N", "1.443565 E"));
//
//   // 121
//   // http://api.geonames.org/srtm1?lat=47.845151&lng=2.755539&username=jeromev
//     Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
//
//     //799
//     //http://api.geonames.org/srtm1?lat=43.148494&lng=-6.555347&username=jeromev
//     Serial.println(getGroundLevel("43.148494 N", "6.555347 W"));
//
//     //Dome du gouter 4292
//   // http://api.geonames.org/srtm1?lat=45.842779&lng=6.843396&username=jeromev
//
//   Serial.println(getGroundLevel("45.842779 N", "6.843396 E"));
//
//   // Roc'h Ruz 361
//   // http://api.geonames.org/srtm1?lat=48.413305&lng=-3.889676&username=jeromev
//
//   Serial.println(getGroundLevel("48.413305 N", "3.889676 W"));
//
//   // PDD 1411
//   //http://api.geonames.org/srtm1?lat=45.772282&lng=2.961406&username=jeromev
//   Serial.println(getGroundLevel("45.772282 N", "2.961406 E"));
//
//   //2916
//   //http://api.geonames.org/srtm1?lat=42.591652&lng=1.443565&username=jeromev
//   Serial.println(getGroundLevel("42.591652 N", "1.443565 E"));
//
//   // 121
//   // http://api.geonames.org/srtm1?lat=47.845151&lng=2.755539&username=jeromev
//     Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
//
//     //799
//     //http://api.geonames.org/srtm1?lat=43.148494&lng=-6.555347&username=jeromev
//     Serial.println(getGroundLevel("43.148494 N", "6.555347 W"));
//
// //Dome du gouter 4292
//   // http://api.geonames.org/srtm1?lat=45.842779&lng=6.843396&username=jeromev
//
//   Serial.println(getGroundLevel("45.842779 N", "6.843396 E"));
//
//   // Roc'h Ruz 361
//   // http://api.geonames.org/srtm1?lat=48.413305&lng=-3.889676&username=jeromev
//
//   Serial.println(getGroundLevel("48.413305 N", "3.889676 W"));
//
//   // PDD 1411
//   //http://api.geonames.org/srtm1?lat=45.772282&lng=2.961406&username=jeromev
//   Serial.println(getGroundLevel("45.772282 N", "2.961406 E"));
//
//   //2916
//   //http://api.geonames.org/srtm1?lat=42.591652&lng=1.443565&username=jeromev
//   Serial.println(getGroundLevel("42.591652 N", "1.443565 E"));
//
//   // 121
//   // http://api.geonames.org/srtm1?lat=47.845151&lng=2.755539&username=jeromev
//     Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
//
//     //799
//     //http://api.geonames.org/srtm1?lat=43.148494&lng=-6.555347&username=jeromev
//     Serial.println(getGroundLevel("43.148494 N", "6.555347 W"));
//
// //Dome du gouter 4292
//   // http://api.geonames.org/srtm1?lat=45.842779&lng=6.843396&username=jeromev
//
//   Serial.println(getGroundLevel("45.842779 N", "6.843396 E"));
//
//   // Roc'h Ruz 361
//   // http://api.geonames.org/srtm1?lat=48.413305&lng=-3.889676&username=jeromev
//
//   Serial.println(getGroundLevel("48.413305 N", "3.889676 W"));
//
//   // PDD 1411
//   //http://api.geonames.org/srtm1?lat=45.772282&lng=2.961406&username=jeromev
//   Serial.println(getGroundLevel("45.772282 N", "2.961406 E"));
//
//   //2916
//   //http://api.geonames.org/srtm1?lat=42.591652&lng=1.443565&username=jeromev
//   Serial.println(getGroundLevel("42.591652 N", "1.443565 E"));
//
//   // 121
//   // http://api.geonames.org/srtm1?lat=47.845151&lng=2.755539&username=jeromev
//     Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
//
//     //799
//     //http://api.geonames.org/srtm1?lat=43.148494&lng=-6.555347&username=jeromev
//     Serial.println(getGroundLevel("43.148494 N", "6.555347 W"));
//
     unsigned long time2 = millis();
//
     Serial.printf(" durée de calcul : %d ms \n", time2 - time1);
}

void trucBis()
{
    unsigned long time1 = millis();

    //Dome du gouter 4292
  // http://api.geonames.org/srtm1?lat=45.842779&lng=6.843396&username=jeromev

  Serial.println(getGroundLevel(45.842779, 6.843396));

  // Roc'h Ruz 361
  // http://api.geonames.org/srtm1?lat=48.413305&lng=-3.889676&username=jeromev

   Serial.println(getGroundLevel(48.413305, -3.889676));
//
//   // PDD 1411
//   //http://api.geonames.org/srtm1?lat=45.772282&lng=2.961406&username=jeromev
//   Serial.println(getGroundLevel("45.772282 N", "2.961406 E"));
//
//   //2916
//   //http://api.geonames.org/srtm1?lat=42.591652&lng=1.443565&username=jeromev
//   Serial.println(getGroundLevel("42.591652 N", "1.443565 E"));
//
//   // 121
//   // http://api.geonames.org/srtm1?lat=47.845151&lng=2.755539&username=jeromev
//     Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
//
//     //799
//     //http://api.geonames.org/srtm1?lat=43.148494&lng=-6.555347&username=jeromev
//     Serial.println(getGroundLevel("43.148494 N", "6.555347 W"));
//
//     //Dome du gouter 4292
//   // http://api.geonames.org/srtm1?lat=45.842779&lng=6.843396&username=jeromev
//
//   Serial.println(getGroundLevel("45.842779 N", "6.843396 E"));
//
//   // Roc'h Ruz 361
//   // http://api.geonames.org/srtm1?lat=48.413305&lng=-3.889676&username=jeromev
//
//   Serial.println(getGroundLevel("48.413305 N", "3.889676 W"));
//
//   // PDD 1411
//   //http://api.geonames.org/srtm1?lat=45.772282&lng=2.961406&username=jeromev
//   Serial.println(getGroundLevel("45.772282 N", "2.961406 E"));
//
//   //2916
//   //http://api.geonames.org/srtm1?lat=42.591652&lng=1.443565&username=jeromev
//   Serial.println(getGroundLevel("42.591652 N", "1.443565 E"));
//
//   // 121
//   // http://api.geonames.org/srtm1?lat=47.845151&lng=2.755539&username=jeromev
//     Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
//
//     //799
//     //http://api.geonames.org/srtm1?lat=43.148494&lng=-6.555347&username=jeromev
//     Serial.println(getGroundLevel("43.148494 N", "6.555347 W"));
//
// //Dome du gouter 4292
//   // http://api.geonames.org/srtm1?lat=45.842779&lng=6.843396&username=jeromev
//
//   Serial.println(getGroundLevel("45.842779 N", "6.843396 E"));
//
//   // Roc'h Ruz 361
//   // http://api.geonames.org/srtm1?lat=48.413305&lng=-3.889676&username=jeromev
//
//   Serial.println(getGroundLevel("48.413305 N", "3.889676 W"));
//
//   // PDD 1411
//   //http://api.geonames.org/srtm1?lat=45.772282&lng=2.961406&username=jeromev
//   Serial.println(getGroundLevel("45.772282 N", "2.961406 E"));
//
//   //2916
//   //http://api.geonames.org/srtm1?lat=42.591652&lng=1.443565&username=jeromev
//   Serial.println(getGroundLevel("42.591652 N", "1.443565 E"));
//
//   // 121
//   // http://api.geonames.org/srtm1?lat=47.845151&lng=2.755539&username=jeromev
//     Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
//
//     //799
//     //http://api.geonames.org/srtm1?lat=43.148494&lng=-6.555347&username=jeromev
//     Serial.println(getGroundLevel("43.148494 N", "6.555347 W"));
//
// //Dome du gouter 4292
//   // http://api.geonames.org/srtm1?lat=45.842779&lng=6.843396&username=jeromev
//
//   Serial.println(getGroundLevel("45.842779 N", "6.843396 E"));
//
//   // Roc'h Ruz 361
//   // http://api.geonames.org/srtm1?lat=48.413305&lng=-3.889676&username=jeromev
//
//   Serial.println(getGroundLevel("48.413305 N", "3.889676 W"));
//
//   // PDD 1411
//   //http://api.geonames.org/srtm1?lat=45.772282&lng=2.961406&username=jeromev
//   Serial.println(getGroundLevel("45.772282 N", "2.961406 E"));
//
//   //2916
//   //http://api.geonames.org/srtm1?lat=42.591652&lng=1.443565&username=jeromev
//   Serial.println(getGroundLevel("42.591652 N", "1.443565 E"));
//
//   // 121
//   // http://api.geonames.org/srtm1?lat=47.845151&lng=2.755539&username=jeromev
//     Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
//
//     //799
//     //http://api.geonames.org/srtm1?lat=43.148494&lng=-6.555347&username=jeromev
//     Serial.println(getGroundLevel("43.148494 N", "6.555347 W"));
//
     unsigned long time2 = millis();
//
     Serial.printf(" durée de calcul : %d ms \n", time2 - time1);
}

void truc2()
{
    unsigned long time1 = millis();


    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
      unsigned long time2 = millis();
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));
    Serial.println(getGroundLevel("47.845151 N", "2.755539 E"));

    unsigned long time3 = millis();

    Serial.printf(" durée de calcul 1: %d ms \n", time2 - time1);
    Serial.printf(" durée de calcul 2: %d ms \n", time3 - time1);
}


void loop() {
  // put your main code here, to run repeatedly:


}
