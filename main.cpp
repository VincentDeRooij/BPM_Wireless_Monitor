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
#include <stdint.h>
#include "MAX30102.h" // uses i2c-dev, instead of Wire.h 
#include <sys/time.h>

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
uint8_t rates[RATE_SIZE]; //Array of heart rates
uint8_t rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

void setup()
{
  // Initialize sensor
  if (!particleSensor.begin()) //Use default I2C port, 400kHz speed
  {
    std::cout << "MAX30105 was not found. Please check wiring/power. " << std::endl;
    while (1);
  }
  std::cout << "Place your index finger on the sensor with steady pressure." << std::endl;

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  //particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop()
{
  while (1)
  {
   

  long irValue = particleSensor.getIR();

  struct timeval time_now{};
  gettimeofday(&time_now, nullptr);
  time_t millisecs = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millisecs - lastBeat;
    gettimeofday(&time_now, nullptr);
    millisecs = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
    lastBeat = millisecs;

    beatsPerMinute = 60 / (delta / 1000.0);

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

  if (irValue < 50000)
    std::cout << " No finger?";

  std::cout << std::endl; 
  }
}


