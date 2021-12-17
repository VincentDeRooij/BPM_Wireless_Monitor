/*
    This library is created with the help of the SparkFun MAX30102 library

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
  -5V = 5V
  -GND = GND
  -SDA = SDA on Raspberry Pi
  -SCL = SCL on Raspberry Pi
  -INT = Not connected

  The MAX30105 Breakout can handle 5V or 3.3V I2C logic. We recommend powering the board with 5V
  but it will also run at 3.3V.
*/

#include <iostream>
#include "MAX30102.h" // uses i2c-dev, instead of Wire.h 
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>

MAX30102 particleSensor;
struct timeval __millis_start;

const uint8_t RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
uint8_t rates[RATE_SIZE]; //Array of heart rates
uint8_t rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

int beatsPerMinute;
int beatAvg;

void init_millis() {
    gettimeofday(&__millis_start, NULL);
};

unsigned long int millis() {
    long mtime, seconds, useconds; 
    struct timeval end;
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - __millis_start.tv_sec;
    useconds = end.tv_usec - __millis_start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    return mtime;
};

void setup()
{
  // inits the counter
  init_millis();
  // Initialize sensor
  if (!particleSensor.begin()) //Use default I2C port, 400kHz speed
  {
    std::cout << "MAX30105 was not found. Please check wiring/power. " << std::endl;
    while (1);
  }
  std::cout << "Place your index finger on the sensor with steady pressure." << std::endl;

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
}

void loop()
{
  while (1)
  {
   

  long irValue = particleSensor.getIR();

  if(irValue > 100000)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    std::cout << "DELTA: " << delta << std::endl;

    beatsPerMinute = 60 / (delta / 1000);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (uint8_t)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (uint8_t x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  std::cout << "IR=";
  std::cout << irValue;
  std::cout << ", BPM=";
  std::cout << beatsPerMinute;
  std::cout << ", Avg BPM=";
  std::cout << beatAvg;
  std::cout << ", Last Millis()=";
  std::cout << lastBeat;

  if (irValue < 50000)
    std::cout << " No finger?";

  std::cout << std::endl;
  }
}


int main()
{
  setup();
  loop();
}
