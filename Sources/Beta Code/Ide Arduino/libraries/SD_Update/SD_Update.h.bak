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
/*  version    Date        Description                                           */
/*    1.0                                                                        */
/*    1.1      29/11/19    Modif sdfat                                           */
/*    1.1.1    15/12/19    Ajout TRACE                                           */
/*                         Modification performUpdate                            */
/*    1.1.2    30/01/20    Ajout écran d'info lors de l'upgrade                  */
/*                                                                               */
/*********************************************************************************/

#ifndef SD_UPDATE_H
#define SD_UPDATE_H

#include <Arduino.h>
#include <HardwareConfig.h>
#include <DebugConfig.h>

#include <varioscreenGxEPD.h>

#ifndef ARDUINOTRACE_ENABLE
#ifdef SDCARD_DEBUG
#define ARDUINOTRACE_ENABLE 1
#else
#define ARDUINOTRACE_ENABLE 0
#endif
#endif

#ifndef ARDUINOTRACE_SERIAL
#define ARDUINOTRACE_SERIAL SerialPort
#endif

#include <ArduinoTrace.h>

// perform the actual update from a given stream
void performUpdate(Stream &updateSource, size_t updateSize)
{
   if (Update.begin(updateSize))
   {
      size_t written = Update.writeStream(updateSource);
      if (written == updateSize)
      {
         SerialPort.println("Written : " + String(written) + " successfully");
      }
      else
      {
         SerialPort.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
      }
      if (Update.end())
      {
         SerialPort.println("OTA done!");
         if (Update.isFinished())
         {
            SerialPort.println("Update successfully completed. Rebooting.");
         }
         else
         {
            SerialPort.println("Update not finished? Something went wrong!");
         }
      }
      else
      {
         SerialPort.println("Error Occurred. Error #: " + String(Update.getError()));
      }
   }
   else
   {
      SerialPort.println("Not enough space to begin OTA");
   }
}

#if defined(SDFAT_LIB)

void performUpdate(SdFile &updateFile, size_t updateSize)
{
   if (Update.begin(updateSize))
   {

      size_t written = 0;

      while (updateFile.available())
      {
         uint8_t ibuffer[128];
         updateFile.read((uint8_t *)ibuffer, 128);
         //			SerialPort.println((char *)ibuffer);
         Update.write(ibuffer, sizeof(ibuffer));
         written += sizeof(ibuffer);
      }

      //		size_t written = Update.writeStream(updateSource);
      if (written == updateSize)
      {
         SerialPort.println("Written : " + String(written) + " successfully");
      }
      else
      {
         SerialPort.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
      }
      if (Update.end())
      {
         SerialPort.println("OTA done!");
         if (Update.isFinished())
         {
            SerialPort.println("Update successfully completed. Rebooting.");
         }
         else
         {
            SerialPort.println("Update not finished? Something went wrong!");
         }
      }
      else
      {
         SerialPort.println("Error Occurred. Error #: " + String(Update.getError()));
      }
   }
   else
   {
      SerialPort.println("Not enough space to begin OTA");
   }
}
#endif //SDFAT_LIB

// check given FS for valid update.bin and perform update if available
void updateFromSDCARD()
{
#ifdef HAVE_SDCARD
   if (SDHAL_SD.exists("/update.bin"))
   {
#ifdef SDFAT_LIB
      TRACE();
      SdFile updateBin;
      if (updateBin.open("update.bin", O_RDONLY))
      {
         if (updateBin.isDir())
         {
#else  //SDFAT_LIB
      File updateBin = SDHAL_SD.open("/update.bin");
      if (updateBin)
      {
         if (updateBin.isDirectory())
         {
#endif //SDFAT_LIB
            SerialPort.println("Error, update.bin is not a file");
            updateBin.close();
            return;
         }

         screen.ScreenViewReboot("Upgrade");

#ifdef SDFAT_LIB
         size_t updateSize = updateBin.fileSize();
#else  //SDFAT_LIB
         size_t updateSize = updateBin.size();
#endif //SDFAT_LIB
         DUMP(updateSize);

         if (updateSize > 0)
         {
            SerialPort.println("Try to start update");
#ifdef SDFAT_LIB
            performUpdate(updateBin, updateSize);
#else  //SDFAT_LIB*/
            performUpdate(updateBin, updateSize);
#endif //SDFAT_LIB
         }
         else
         {
            SerialPort.println("Error, file is empty");
            updateBin.close();
            return;
         }

         updateBin.close();

         // whe finished remove the binary from sd card to indicate end of the process
#ifdef SDFAT_LIB
         SDHAL_SD.remove("update.bin");
#else  //SDFAT_LIB*/
         SDHAL_SD.remove("/update.bin");
#endif //SDFAT_LIB

         SerialPort.println("RESTART ESP32");
         SerialPort.flush();
         ESP_LOGI("GnuVario-E", "RESTART ESP32");
         screen.ScreenViewReboot();
         ESP.restart();
      }
      else
      {
         SerialPort.println("Could not load update.bin from sd root");
      }
   }
   else
   {
      SerialPort.println("No update file.");
   }
#endif
}

#endif