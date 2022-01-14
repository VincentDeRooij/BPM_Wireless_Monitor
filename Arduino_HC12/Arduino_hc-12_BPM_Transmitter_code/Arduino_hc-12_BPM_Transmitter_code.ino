/*
  With Updates for the HC-12 code, 
  combining this we get a transmitter which uses the HC-12 module to send BPM data to the Raspberry Pi via another HC-12 module
  
  Optical Heart Rate Detection (PBA Algorithm) using the MAX30105 Breakout
  By: Nathan Seidle @ SparkFun Electronics
  Date: October 2nd, 2016
  https://github.com/sparkfun/MAX30105_Breakout

  This is a demo to show the reading of heart rate or beats per minute (BPM) using
  a Penpheral Beat Amplitude (PBA) algorithm.

  It is best to attach the sensor to your finger using a rubber band or other tightening
  device. Humans are generally bad at applying constant pressure to a thing. When you
  press your finger against the sensor it varies enough to cause the blood in your
  finger to flow differently which causes the sensor readings to go wonky.

  Hardware Connections (Breakoutboard to Arduino):
  -5V = 5V (3.3V is allowed)
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  The MAX30105 Breakout can handle 5V or 3.3V I2C logic. We recommend powering the board with 5V
  but it will also run at 3.3V.
*/

// includes for the BPM pulse oxy-meter
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

// include for the HC-12 module
#include <SoftwareSerial.h>

// BPM pulse oxy-meter defines and variables
MAX30105 particleSensor;

const byte RATE_SIZE = 8; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;


// HC-12 defines and variables
#define rxPin 2
#define txPin 3
#define UART_BAUDRATE 9600

#define TRANSMITTER_READINGS_TRIGGER 1000 // if 1000 readings were triggered then proceed to sent a current update of the avg bpm 

struct SerialControllerDataPack
{
    int data;
};

SerialControllerDataPack pack;

SoftwareSerial HC12(rxPin, txPin);

int startMillis;

bool calcIfCorrectMillisReached()
{
  int deltaMillis = millis() - startMillis;
  
  if(deltaMillis >= 2000)
  {
    return true;
  }
  return false;
}

void setup()
{
  // define pin modes for tx, rx for the HC-12 module:
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  // Setup Serial Monitor
  Serial.begin(115200);
  Serial.println("Initializing...");

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Serial monitor available... OK");

  Serial.print("Serial link available... ");

  HC12.begin(UART_BAUDRATE);
  if (HC12.isListening()) { // if the HC-12 module is up and running
    Serial.println("HC-12 OK!");
  } else {
    Serial.println("HC-12 N_OK!");
  }
  
  //test HC-12
  HC12.println("PROBE_MESSAGE_TEST"); // Testing the message and link to the RASPBERRY PI
  Serial.print("HC-12 available... ");
  Serial.println("initialization done.");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

  // fill startMillis
  startMillis = millis(); // get current time of the millis
}

void loop()
{
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  Serial.println();

  if(calcIfCorrectMillisReached() == true)
  {
    HC12.println(beatAvg); // send the avg beat
    Serial.println("Send!");
    startMillis = millis();
  }

  if (irValue < 50000)
    Serial.print(" No finger?");

}
