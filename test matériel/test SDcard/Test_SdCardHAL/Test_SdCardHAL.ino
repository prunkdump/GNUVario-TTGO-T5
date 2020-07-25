
#include <sdcardHAL.h>

#include <HardwareConfig.h>

#include <DebugConfig.h>

#ifdef SDFAT_LIB
    SdFile root;
#else //SDFAT_LIB
    File root;
#endif //SDFAT_LIB


void setup()
{
  SerialPort.begin(115200);
  delay(5000);
  SerialPort.print("Initializing SD card...");
  /* initialize SD library with SPI pins */
 if (!SDHAL.begin()) {            //T1:13,15,2,14  T2: 23,5,19,18
    SerialPort.println("initialization failed!");
    return;
  }
  SerialPort.println("initialization done.");
  /* Begin at the root "/" */

#ifdef SDFAT_LIB
  if (root.open("/", O_RDONLY)) {
#else //SDFAT_LIB
  root = SDHAL_SD.open("/");
  if (root) {
#endif //SDFAT_LIB    
    printDirectory(root, 0);
    root.rewindDirectory();
    root.close();
  } else {
    SerialPort.println("error opening test.txt");
  }
  /* open "test.txt" for writing */
  /* if open succesfully -> root != NULL 
    then write string "Hello world!" to it
  */
#ifdef SDFAT_LIB
  if (root.open("/test.txt", O_WRONLY | O_CREAT)) {
#else //SDFAT_LIB
  root = SDHAL_SD.open("/test.txt", FILE_WRITE);
  if (root) {
#endif //SDFAT_LIB    

    root.println("Hello world!");
    root.flush();
   /* close the file */
    root.close();
  } else {
    /* if the file open error, print an error */
    SerialPort.println("error opening test.txt");
  }
  delay(1000);
  /* after writing then reopen the file and read it */

#ifdef SDFAT_LIB
  if (root.open("/test.txt", O_RDONLY)) {
#else //SDFAT_LIB
  root = SDHAL_SD.open("/test.txt");
  if (root) {
#endif //SDFAT_LIB    
    /* read from the file until there's nothing else in it */
    while (root.available()) {
      /* read the file and print to Terminal */
      SerialPort.write(root.read());
    }
    root.close();
  } else {
    SerialPort.println("error opening test.txt");
  }
  
  SerialPort.println("done!");
}

void loop()
{
}

void printDirectory(File dir, int numTabs) {
  
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
       printDirectory(entry, numTabs+1);
     } else {
       /* files have sizes, directories do not */
       SerialPort.print("\t\t");
       SerialPort.println(entry.size());
     }
     entry.close();
   }
}
