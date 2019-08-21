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
}