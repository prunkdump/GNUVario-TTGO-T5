#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <MS5xxx.h>

// internal LED to show conection
#define LED_INT 9
// typical I2C-Address of chip
#define I2C_MS5607 0x76
// I2C commands of chip
#define CMD_RESET   0x1E     // perform reset
#define CMD_ADC_READ 0x00    // initiate read sequence
#define CMD_ADC_CONV 0x40    // start conversion
#define CMD_ADC_D1   0x00    // read ADC 1
#define CMD_ADC_D2   0x10    // read ADC 2
#define CMD_ADC_256  0x00    // set ADC oversampling ratio to 256
#define CMD_ADC_512  0x02    // set ADC oversampling ratio to 512
#define CMD_ADC_1024 0x04    // set ADC oversampling ratio to 1024
#define CMD_ADC_2048 0x06    // set ADC oversampling ratio to 2048
#define CMD_ADC_4096 0x08    // set ADC oversampling ratio to 4096
#define CMD_PROM_RD  0xA0    // initiate readout of PROM registers

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 0, 0, 6);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 0, 0, 0);

MS5xxx sensor(&Wire);
EthernetServer server(23);
boolean alreadyConnected = false; // whether or not the client was connected previously

double P;
double TEMP;

long lastReadingTime = 0;
unsigned int cnt=0;
double AVG_P=0;
double AVG_T=0;

boolean sc_state=false;

void init_sens() {
  pinMode(LED_INT, OUTPUT);
  if(sensor.connect()>0) {
    Serial.println("Error connecting...");
    digitalWrite(LED_INT, LOW);
    delay(500);
    init_sens();
  }
  else {
    digitalWrite(LED_INT, HIGH);
    sensor.ReadProm();    
  }
}

void setup() {
  Serial.begin(9600);  
  init_sens();

  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
}


void loop() {
  // wait for a new client:
  EthernetClient client = server.available();

  if (client) {
    if (!alreadyConnected) {
      // clear the input buffer:
      client.flush();    
      client.println("MS5607 readout initialized !"); 
      alreadyConnected = true;
    } 

    char cli_in = client.read();
    unsigned int diff = millis() - lastReadingTime;
    
    if(cli_in=='d') {
      sensor.Readout();
      client.print(sensor.GetTemp(),2);
      client.print(";");
      client.print(sensor.GetPres(),2);
      client.print("\r");
      client.print("\n");
    }  
  }
}

