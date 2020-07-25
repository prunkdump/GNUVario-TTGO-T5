
#include <sdcardHAL.h>

#include <HardwareConfig.h>

#include <DebugConfig.h>

SdFile root;

void setup()
{
  SerialPort.begin(115200);

  while (!Serial) {
    SysCall::yield();
  }

  delay(5000);

#ifdef pinLED
   pinMode(pinLED, OUTPUT);
#endif

  SerialPort.println("Initializing SD card...");
  /* initialize SD library with SPI pins */
 if (!SDHAL.begin()) {            //T1:13,15,2,14  T2: 23,5,19,18
    SerialPort.println("initialization failed!");
    return;
  }
  SerialPort.println("initialization done.");

  
  /* Begin at the root "/" */
  if (root.open("/")) {    
    printDirectory(root, 0);
    root.close();
  } else {
    SerialPort.println("error opening test.txt");
  }
  
  /* open "test.txt" for writing */

  /* if open succesfully  
    then write string "Hello world!" to it
  */
  if (root.open("test.txt", O_RDWR | O_CREAT)) {
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
  if (root.open("test.txt", O_RDONLY)) {    
    /* read from the file until there's nothing else in it *
    while (root.available()) {
      /* read the file and print to Terminal */
    while (root.available()) {
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
#ifdef pinLED
  digitalWrite(pinLED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(pinLED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
#else
  delay(10);
#endif
}

void printDirectory(SdFile dir, int numTabs) {

  SdFile entry;
  char fBuffer[15];
 
  while(true) {
     
 /* while (file.openNext(&dir, O_RDONLY)) {
    if (!file.isHidden()) {
      rootFileCount++;
    }*/
     
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
       printDirectory(entry, numTabs+1);
     } else {
       /* files have sizes, directories do not */
       SerialPort.print("\t\t");
       SerialPort.println(entry.fileSize());
     }
     entry.close();
   }
}


/*
   if (sd.exists("Folder1")
    || sd.exists("Folder1/file1.txt")
    || sd.exists("Folder1/File2.txt")) {
    error("Please remove existing Folder1, file1.txt, and File2.txt");
  }
 */
