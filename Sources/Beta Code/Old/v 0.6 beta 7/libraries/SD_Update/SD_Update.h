/* SD_Update -- 
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
/*                           VarioCalibration                                    */
/*                                                                               */
/*  version    Date     Description                                              */
/*    1.0                                                                        */
/*                                                                               */
/*********************************************************************************/


#ifndef SD_UPDATE_H
#define SD_UPDATE_H

#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

// perform the actual update from a given stream
void performUpdate(Stream &updateSource, size_t updateSize) {
   if (Update.begin(updateSize)) {      
      size_t written = Update.writeStream(updateSource);
      if (written == updateSize) {
         SerialPort.println("Written : " + String(written) + " successfully");
      }
      else {
         SerialPort.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
      }
      if (Update.end()) {
         SerialPort.println("OTA done!");
         if (Update.isFinished()) {
            SerialPort.println("Update successfully completed. Rebooting.");
         }
         else {
            SerialPort.println("Update not finished? Something went wrong!");
         }
      }
      else {
        SerialPort.println("Error Occurred. Error #: " + String(Update.getError()));
      }
   }
   else
   {
     SerialPort.println("Not enough space to begin OTA");
   }
}

// check given FS for valid update.bin and perform update if available
void updateFromSDCARD() {
#ifdef HAVE_SDCARD
   File updateBin = SDHAL.open("/update.bin");
   if (updateBin) {
      if(updateBin.isDirectory()){
         SerialPort.println("Error, update.bin is not a file");
         updateBin.close();
         return;
      }

      size_t updateSize = updateBin.size();

      if (updateSize > 0) {
         SerialPort.println("Try to start update");
         performUpdate(updateBin, updateSize);
      }
      else {
         SerialPort.println("Error, file is empty");
      }

      updateBin.close();
    
      // whe finished remove the binary from sd card to indicate end of the process
     SDHAL.remove("/update.bin");      
   }
   else {
      Serial.println("Could not load update.bin from sd root");
   }
#endif
}

#endif