/* utility -- 
 *
 * Copyright 2019 Jean-philippe GOI
 * 
 * This file is part of GnuVario-E.
 *
 * ToneHAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ToneHAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*********************************************************************************/
/*                                                                               */
/*                           Utility                                             */
/*                                                                               */
/*  version    Date        Description                                           */
/*    1.0      05/07/19                                                          */
/*    1.1      24/09/19    Ajout deep_sleep                                      */
/*    1.2      15/10/19    Ajout test SDCARD                                     */
/*    1.3      23/11/19    Modification deep sleep                               */
/*    1.4      28/11/19    Modif changement librairie sdfat                      */
/*    1.5      06/10/20    Modification deep_sleep                               */
/*                                                                               */
/*********************************************************************************/

#include <Arduino.h>

#include <Utility.h>
#include <DebugConfig.h>
#include <HardwareConfig.h>

#ifdef HAVE_SDCARD
#include <sdcardHAL.h>
#endif //HAVE_SDCARD

#include <VarioSettings.h>

#ifdef HAVE_SPEAKER
#include <toneHAL.h>
#include <beeper.h>
#endif //HAVE_SPEAKER

#ifdef HAVE_SCREEN
#include <varioscreenGxEPD.h>
#endif

#include "VarioImuTwoWire.h"

#include "driver/rtc_io.h"

/**********************/
/* sensor objects */
/**********************/

// if imu calibration data in flash is corrupted, the accel and gyro biases are 
// set to 0, and this uncalibrated state is indicated with a sequence of alternating 
// low and high beeps.
void indicateUncalibratedAccelerometer() {
#ifdef HAVE_SPEAKER
  int tmpvolume = beeper.getVolume();	
  beeper.setVolume(10);
  for (int cnt = 0; cnt < 5; cnt++) {
    beeper.generateTone(200,100); 
    beeper.generateTone(2000,100);
    }
  beeper.setVolume(tmpvolume);	
#endif //HAVE_SPEAKER
  }

// "no-activity" power down is indicated with a series of descending
// tones. If you hear this, switch off the vario as there is still
// residual current draw from the circuit components  
void indicatePowerDown() {
#ifdef HAVE_SPEAKER
  beeper.generateTone(2000,1000); 
  beeper.generateTone(1000,1000);
  beeper.generateTone(500, 1000);
  beeper.generateTone(250, 1000);
#endif //HAVE_SPEAKER
  }

// problem with MS5611 calibration CRC, assume communication 
// error or bad device. Indicate with series of 10 high pitched beeps.
void indicateFaultMS5611() {
#ifdef HAVE_SPEAKER
  for (int cnt = 0; cnt < 10; cnt++) {
    beeper.generateTone(GnuSettings.MS5611_ERROR_TONE_FREQHZ,1000); 
    delay(100);
    }
#endif //HAVE_SPEAKER    
  }

// problem reading MPU9250 ID, assume communication 
// error or bad device. Indicate with series of 10 low pitched beeps.
void indicateFaultMPU9250() {
#ifdef HAVE_SPEAKER
  for (int cnt = 0; cnt < 10; cnt++) {
    beeper.generateTone(GnuSettings.MPU9250_ERROR_TONE_FREQHZ,1000); 
    delay(100);
    }
#endif //HAVE_SPEAKER    
  }


// problem SDCARD, assume communication 
// error or bad device. Indicate with series of 10 low pitched beeps.
void indicateFaultSDCARD() {
#ifdef HAVE_SPEAKER
  for (int cnt = 0; cnt < 10; cnt++) {
    beeper.generateTone(GnuSettings.SDCARD_ERROR_TONE_FREQHZ,1000); 
    delay(100);
    }
#endif //HAVE_SPEAKER
  }

/* make beeps */
void signalBeep(double freq, unsigned long duration, int count = 1) {

#ifdef HAVE_SPEAKER 
  toneHAL.tone(freq, 10);
  delay(duration);

  toneHAL.noTone();

  if( count > 1 ) {
    while( count > 1 ) {
      delay(duration);
      toneHAL.tone(freq, 10);

      delay(duration);

      toneHAL.noTone();

      count--;
    }
  }
#endif //HAVE_SPEAKER  
}
	
#define cellfull 4.2
#define cellempty 3	
	
//--## Tableau de decharge LIPO	
//{{3.000, 0}, {3.053, 1}, {3.113, 2}, {3.174, 3}, {3.237, 4}, {3.300, 5}, {3.364, 6}, {3.427, 7}, {3.488, 8}, {3.547, 9}, {3.600, 10}, {3.621, 11}, {3.637, 12}, {3.649, 13}, {3.659, 14}, {3.668, 15}, {3.676, 16}, {3.683, 17}, {3.689, 18}, {3.695, 19}, {3.700, 20}, {3.706, 21}, {3.712, 22}, {3.717, 23}, {3.723, 24}, {3.728, 25}, {3.732, 26}, {3.737, 27}, {3.741, 28}, {3.746, 29}, {3.750, 30}, {3.754, 31}, {3.759, 32}, {3.763, 33}, {3.767, 34}, {3.771, 35}, {3.775, 36}, {3.779, 37}, {3.782, 38}, {3.786, 39}, {3.790, 40}, {3.794, 41}, {3.798, 42}, {3.802, 43}, {3.806, 44}, {3.810, 45}, {3.814, 46}, {3.818, 47}, {3.822, 48}, {3.826, 49}, {3.830, 50}, {3.834, 51}, {3.838, 52}, {3.842, 53}, {3.846, 54}, {3.850, 55}, {3.854, 56}, {3.858, 57}, {3.862, 58}, {3.866, 59}, {3.870, 60}, {3.875, 61}, {3.880, 62}, {3.885, 63}, {3.890, 64}, {3.895, 65}, {3.900, 66}, {3.905, 67}, {3.910, 68}, {3.915, 69}, {3.920, 70}, {3.924, 71}, {3.929, 72}, {3.933, 73}, {3.938, 74}, {3.942, 75}, {3.947, 76}, {3.952, 77}, {3.958, 78}, {3.963, 79}, {3.970, 80}, {3.982, 81}, {3.994, 82}, {4.007, 83}, {4.020, 84}, {4.033, 85}, {4.047, 86}, {4.060, 87}, {4.074, 88}, {4.087, 89}, {4.100, 90}, {4.111, 91}, {4.122, 92}, {4.132, 93}, {4.143, 94}, {4.153, 95}, {4.163, 96}, {4.173, 97}, {4.182, 98}, {4.191, 99}, {4.200, 100}} --## Table standard (Empirique & théorique)
//{{2.8, 0}, {2.942, 1}, {3.1, 2}, {3.258, 3}, {3.401, 4}, {3.485, 5}, {3.549, 6}, {3.601, 7}, {3.637, 8}, {3.664, 9}, {3.679, 10}, {3.683, 11}, {3.689, 12}, {3.692, 13}, {3.705, 14}, {3.71, 15}, {3.713, 16}, {3.715, 17}, {3.72, 18}, {3.731, 19}, {3.735, 20}, {3.744, 21}, {3.753, 22}, {3.756, 23}, {3.758, 24}, {3.762, 25}, {3.767, 26}, {3.774, 27}, {3.78, 28}, {3.783, 29}, {3.786, 30}, {3.789, 31}, {3.794, 32}, {3.797, 33}, {3.8, 34}, {3.802, 35}, {3.805, 36}, {3.808, 37}, {3.811, 38}, {3.815, 39}, {3.818, 40}, {3.822, 41}, {3.826, 42}, {3.829, 43}, {3.833, 44}, {3.837, 45}, {3.84, 46}, {3.844, 47}, {3.847, 48}, {3.85, 49}, {3.854, 50}, {3.857, 51}, {3.86, 52}, {3.863, 53}, {3.866, 54}, {3.87, 55}, {3.874, 56}, {3.879, 57}, {3.888, 58}, {3.893, 59}, {3.897, 60}, {3.902, 61}, {3.906, 62}, {3.911, 63}, {3.918, 64}, {3.923, 65}, {3.928, 66}, {3.939, 67}, {3.943, 68}, {3.949, 69}, {3.955, 70}, {3.961, 71}, {3.968, 72}, {3.974, 73}, {3.981, 74}, {3.987, 75}, {3.994, 76}, {4.001, 77}, {4.008, 78}, {4.014, 79}, {4.021, 80}, {4.029, 81}, {4.036, 82}, {4.044, 83}, {4.052, 84}, {4.062, 85}, {4.074, 86}, {4.085, 87}, {4.095, 88}, {4.105, 89}, {4.111, 90}, {4.116, 91}, {4.12, 92}, {4.125, 93}, {4.129, 94}, {4.135, 95}, {4.145, 96}, {4.176, 97}, {4.179, 98}, {4.193, 99}, {4.2, 100}}                 --## Table Robbe originale fiable (Départ à 2.8V
double myArrayPercentList[101] =                                                       
              {3, 3.093, 3.196, 3.301, 3.401, 3.477, 3.544, 3.601, 3.637, 3.664, 3.679, 3.683, 3.689, 3.692, 3.705, 3.71, 
			  3.713, 3.715, 3.72, 3.731, 3.735, 3.744, 3.753, 3.756, 3.758, 3.762, 3.767, 3.774, 3.78, 3.783, 3.786, 3.789, 
			  3.794, 3.797, 3.8, 3.802, 3.805, 3.808, 3.811, 3.815, 3.818, 3.822, 3.825, 3.829, 3.833, 3.836, 3.84, 3.843, 3.847, 3.85, 3.854, 
			  3.857, 3.86, 3.863, 3.866, 3.87, 3.874, 3.879, 3.888, 3.893, 3.897, 3.902, 3.906, 3.911, 3.918, 3.923, 3.928, 3.939, 3.943, 
			  3.949, 3.955, 3.961, 3.968, 3.974, 3.981, 3.987, 3.994, 4.001, 4.007, 4.014, 4.021, 4.029, 4.036, 4.044, 4.052, 4.062, 4.074, 4.085, 4.095, 
			  4.105, 4.111, 4.116, 4.12, 4.125, 4.129, 4.135, 4.145, 4.176, 4.179, 4.193, 4.2};   //## Table Robbe fiable modifiée pour départ à 3.0V

int8_t percentBat(double targetVoltage) {
  int8_t result = 0;
  
#ifdef PROG_DEBUG
       SerialPort.print("percentbat : ");
       SerialPort.println(targetVoltage);
#endif //PROG_DEBUG
  
  if ((targetVoltage > cellfull) or (targetVoltage < cellempty)) {
    if  (targetVoltage > cellfull) {                                            //## trap for odd values not in array
      result = 100;
    }
    if  (targetVoltage < cellempty) {
      result = 0;
    }
  }
  else {
    for (int8_t i=0; i<101; i++) {                                 //## method of finding percent in my array provided by on4mh (Mike)
      if (myArrayPercentList[i] >= targetVoltage) {
        result =  i;
        break;
      }
    } //for
  } //if
  
#ifdef PROG_DEBUG
       SerialPort.print("result : ");
       SerialPort.println(result);
#endif  //PROG_DEBUG
  
 return result;
}

void deep_sleep(String Message) {
	esp_sleep_enable_ext0_wakeup(BUTTON_DEEP_SLEEP,0); //1 = High, 0 = Low

	//If you were to use ext1, you would use it like
	//esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);

	//Go to sleep now
	SerialPort.println("Going to sleep now");
	
	delay(200);

	screen.ScreenViewMessage(Message, 0);

	delay(1000);
	
	#if defined(HAVE_POWER_ALIM) 
	pinMode(POWER_PIN, OUTPUT);
	digitalWrite(POWER_PIN, !POWER_PIN_STATE);   // turn off POWER (POWER_PIN_STATE is the voltage level HIGH/LOW)
	#endif  

	#ifdef HAVE_SDCARD          
	fileIgc.close();
//	SDHAL.end(); 
	#endif

	#ifdef HAVE_AUDIO_AMPLI
	toneHAL.disableAmpli();
	#endif
	//	rtc_gpio_isolate(GPIO_NUM_12);

	twScheduler.disableAcquisition();
	
	rtc_gpio_isolate(GPIO_BUTTON_A);
	rtc_gpio_isolate(GPIO_BUTTON_C);
#ifdef GPIO_MPU_INT
	rtc_gpio_isolate(GPIO_MPU_INT);
#endif

	display.powerOff();
	
	delay(1000);
    SerialPort.println("Sleep now");
	esp_deep_sleep_start();
	SerialPort.println("This will never be printed");		
}

/**********************************/
/*      librairie SdFat           */
/**********************************/

#ifdef SDFAT_LIB

void printSdDirectory(SdFile dir, int numTabs) {
  SdFile entry;
  char fBuffer[15];
	
  while(true) {
     if (!entry.openNext(&dir, O_RDONLY)) {
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       SerialPort.print('\t');   // we'll have a nice indentation
     }
     // Print the name
     entry.getName(fBuffer,13);
     SerialPort.print(fBuffer);
     /* Recurse for directories, otherwise print the file size */
     if (entry.isDir()) {
       SerialPort.println("/");
       printSdDirectory(entry, numTabs+1);
     } else {
       /* files have sizes, directories do not */
       SerialPort.print("\t\t");
       SerialPort.println(entry.fileSize());
     }
     entry.close();
   }
}

//**********************************
// * TEST SDCARD
// **********************************

bool TestSDCARD(bool init) {
	
	if (init) {
		if (!SDHAL.begin()) {
#ifdef SDCARD_DEBUG
			SerialPort.println("initialization failed!");
			return false;
#endif //SDCARD_DEBUG
		}
	}

  SdFile root;
  if (root.open("/")) {    
    printSdDirectory(root, 0);
    root.close();
  } else {
    SerialPort.println("error opening test.txt");
		return false;
  }
  // open "test.txt" for writing 
  // if open succesfully -> root != NULL 
  //  then write string "Hello world!" to it
  
  if (root.open("test.txt", O_RDWR | O_CREAT)) {
    root.println("GnuVario-E Test OK");
    root.flush();
   // close the file 
    root.close();
  } else {
    // if the file open error, print an error 
    SerialPort.println("error write test.txt");
		return false;
  }
	
  delay(1000);
  // after writing then reopen the file and read it 
  if (root.open("test.txt", O_RDONLY)) {    
    // read from the file until there's nothing else in it 
    while (root.available()) {
      // read the file and print to Terminal 
      SerialPort.write(root.read());
    }
    root.close();
  } else {
    SerialPort.println("error read test.txt");
		return false;
  }
  
  SerialPort.println("Test SDcars done!");
	return true;
}

#else
	void printSdDirectory(File dir, int numTabs) {
  
  while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       SerialPort.print('\t');   // we'll have a nice indentation
     }
     // Print the name
     SerialPort.print(entry.name());
     /* Recurse for directories, otherwise print the file size */
     if (entry.isDirectory()) {
       SerialPort.println("/");
       printSdDirectory(entry, numTabs+1);
     } else {
       /* files have sizes, directories do not */
       SerialPort.print("\t\t");
       SerialPort.println(entry.size());
     }
     entry.close();
   }
}

//**********************************
// * TEST SDCARD
// **********************************

bool TestSDCARD(bool init) {
	
	if (init) {
		if (!SDHAL.begin()) {
#ifdef SDCARD_DEBUG
			SerialPort.println("initialization failed!");
			return false;
#endif //SDCARD_DEBUG
		}
	}

  File root = SDHAL_SD.open("/");
  if (root) {    
    printSdDirectory(root, 0);
    root.close();
  } else {
    SerialPort.println("error opening test.txt");
		return false;
  }
  // open "test.txt" for writing 
  root = SDHAL_SD.open("test.txt", FILE_WRITE);
  // if open succesfully -> root != NULL 
  //  then write string "Hello world!" to it
  
  if (root) {
    root.println("GnuVario-E Test OK");
    root.flush();
   // close the file 
    root.close();
  } else {
    // if the file open error, print an error 
    SerialPort.println("error write test.txt");
		return false;
  }
	
  delay(1000);
  // after writing then reopen the file and read it 
  root = SDHAL_SD.open("test.txt");
  if (root) {    
    // read from the file until there's nothing else in it 
    while (root.available()) {
      // read the file and print to Terminal 
      SerialPort.write(root.read());
    }
    root.close();
  } else {
    SerialPort.println("error read test.txt");
		return false;
  }
  
  SerialPort.println("Test SDcars done!");
	return true;
}
#endif