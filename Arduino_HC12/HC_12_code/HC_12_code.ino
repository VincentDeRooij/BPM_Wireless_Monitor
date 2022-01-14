//based on code from: Tom Heylen

#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 3

struct SerialControllerDataPack
{
    int data;
};

SerialControllerDataPack pack;

SoftwareSerial HC12(rxPin, txPin); 
long baud = 9600;

void setup() {
  pack.data = 100;

  // define pin modes for tx, rx:
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  Serial.begin(baud);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Serial monitor available... OK");

  Serial.print("Serial link available... ");
  HC12.begin(baud);
  if (HC12.isListening()) { // if the HC-12 module is up and running
    Serial.println("OK");
  } else {
    Serial.println("N_OK");
  }

  //test HC-12
  Serial.print("HC-12 available... ");
  Serial.println("initialization done.");
}

void loop() {  
  HC12.println(100);
  Serial.println("Send!");
  delay(2000);
}
