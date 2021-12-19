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

// System includes
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

// Heart Sensor includes
#include "MAX30102.h" // uses i2c-dev, instead of Wire.h
#include "heartRate.h"

// TFT display includes
#include "ili9225.h"

// TFT DISPLAY
#define TFT_SCREEN_HEIGHT 220
#define TFT_SCREEN_WIDTH 176
#define OFFSET_X 0
#define OFFSET_Y 0

// Font
#define F_TEXT_MAX_SIZE 20 // max size of the word/sentence
#define FONT_SIZE 32
#define FONT_BUFFER_SIZE (FONT_SIZE * FONT_SIZE / 8)
FontxFile font[2]; // two fonts Width & Heigth

// HEART SENSOR
MAX30102 particleSensor;
struct timeval _millis_start;

#define RATE_SIZE 4       //Increase this for more averaging. 4 is good.
uint8_t rates[RATE_SIZE]; //Array of heart rates
uint8_t rateSpot = 0;
float lastBeat = 0; //Time at which the last beat occurred

int beatsPerMinute;
int beatAvg;

void init_millis()
{
  gettimeofday(&_millis_start, NULL);
};

unsigned long int millis()
{
  long mtime, seconds, useconds;
  struct timeval end;
  gettimeofday(&end, NULL);
  seconds = end.tv_sec - _millis_start.tv_sec;
  useconds = end.tv_usec - _millis_start.tv_usec;

  mtime = ((seconds)*1000 + useconds / 1000.0) + 0.5;
  return mtime;
};

void setup()
{
  Fontx_init(font, "/home/ubuntu/GitRepos/BPM_Wireless_Monitor/FontGraphics/ILGH32XB.FNT", "/home/ubuntu/GitRepos/BPM_Wireless_Monitor/FontGraphics/ILGZ32XB.FNT"); // 16x32Dot Gothic

  // init the TFT Screen
  lcdInit(TFT_SCREEN_WIDTH, TFT_SCREEN_HEIGHT, OFFSET_X, OFFSET_Y);
  lcdReset();
  lcdSetup();

  // inits the counter
  init_millis();
  // Initialize sensor
  if (!particleSensor.begin()) //Use default I2C port, 400kHz speed
  {
    std::cout << "MAX30105 was not found. Please check wiring/power. " << std::endl;
    while (1)
      ;
  }
  std::cout << "Place your index finger on the sensor with steady pressure." << std::endl;

  particleSensor.setup();                    //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
}

void TextTest(const char *txt, int width, int height)
{
  // get font width & height
  uint8_t buffer[FONT_BUFFER_SIZE];
  uint8_t fontWidth;
  uint8_t fontHeight;
  GetFontx(font, 0, buffer, &fontWidth, &fontHeight);
  uint8_t textRefined[F_TEXT_MAX_SIZE];
  strcpy((char *)textRefined, txt);

  uint16_t color;
  lcdFillScreen(BLACK);

  color = RED;
  lcdDrawUTF8String(font, 0, height - fontHeight - 1, textRefined, color);
}

void loop()
{
  while (1)
  {
    long irValue = particleSensor.getIR();

    if (checkForBeat(irValue) == true)
    {
      //We sensed a beat!
      long delta = millis() - lastBeat;
      lastBeat = millis();

      //    std::cout << "DELTA: " << delta << std::endl;

      //failSaveBPM = 60 (delta / 1000.0);

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20)
      {
        rates[rateSpot++] = (uint8_t)beatsPerMinute; //Store this reading in the array
        rateSpot %= RATE_SIZE;                       //Wrap variable

        //Take average of readings
        beatAvg = 0;
        for (uint8_t x = 0; x < RATE_SIZE; x++)
        {
          beatAvg += rates[x];
        }
        beatAvg /= RATE_SIZE;
      }
    }

    std::string val = "BPM: " + std::to_string(beatAvg);
    TextTest(val.c_str(), TFT_SCREEN_WIDTH, TFT_SCREEN_HEIGHT);

    std::cout << ", BPM=";
    std::cout << beatsPerMinute;
    std::cout << ", Avg BPM=";
    std::cout << beatAvg;

    if (irValue < 50000)
      std::cout << " No finger?";

    std::cout << std::endl;

    usleep(1000 * (1000));
  }
}

int main()
{
  setup();
  loop();
}
