// 
// https://www.etechnophiles.com/wp-content/uploads/2021/03/esp32-Board-with-30-pins-Pinout.png

// nRF24
// http://www.learningaboutelectronics.com/Articles/Multiple-SPI-devices-to-an-arduino-microcontroller.php
// https://www.reddit.com/r/esp32/comments/jq3szx/determining_pins_on_esp32/
// http://www.iotsharing.com/2018/03/esp-and-raspberry-connect-with-nrf24l01.html?m=1
// https://www.reddit.com/r/arduino/comments/jcbsco/connect_nrf24l01_to_esp32/
// https://nrf24.github.io/RF24/index.html

// Fix for fake clone
// https://www.youtube.com/watch?v=jfaFbjgwprI
// http://arduino.ah-oui.org/user_docs/dos11/NRF24L01-tutorial.pdf

// TFT ILI9225 2.2inch
// https://github.com/Nkawu/TFT_22_ILI9225

// <<<>>>> NRF24 MODULES INCLUDES
/*
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Brendan Doherty (2bndy5)
 */

/**
 * A simple example of sending data from 1 nRF24L01 transceiver to another.
 *
 * This example was written to be used on 2 devices acting as "nodes".
 * Use the Serial Monitor to change each node's behavior.
 */
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// instantiate an object for the nRF24L01 transceiver
RF24 radio(1, 3); // using pin 7 for the CE pin, and pin 8 for the CSN pin

// Let these addresses be used for the pair
const uint64_t linkerPipe = 0xF0F0F0F0D2LL;
// an identifying device destination

// to use different addresses on a pair of radios, we need a variable to
// uniquely identify which address this radio will use to transmit
bool radioNumber = 0; // 0 uses address[0] to transmit, 1 uses address[1] to transmit

// Used to control whether this node is sending or receiving
bool role = false;  // true = TX role, false = RX role

// For this example, we'll be using a payload containing
// a single float number that will be incremented
// on every successful transmission
float payload = 0.0;


// <<<>>>> TFT DISPLAY INCLUDES
// Include application, user and local libraries
#include "SPI.h"
#include "TFT_22_ILI9225.h"

#define TFT_RST 26  // IO 26
#define TFT_RS  25  // IO 25
#define TFT_CLK 14  // HSPI-SCK
//#define TFT_SDO 12  // HSPI-MISO
#define TFT_SDI 13  // HSPI-MOSI
#define TFT_CS  15  // HSPI-SS0
#define TFT_LED 0   // 0 if wired to +5V directly
SPIClass hspi(HSPI);

#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

// Use hardware SPI (faster - on Uno: 13-SCK, 12-MISO, 11-MOSI)
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_LED, TFT_BRIGHTNESS);

// Variables and constants
uint16_t x, y;
boolean flag = false;

int SESSION_RST_TRIGGER_VAL = 5; // if aprox 10 seconds elapsed
int currentRSTValue = 0;

// BPM Values
int minBPM = 0;
int maxBPM = 0;

void clearDisplay()
{
  tft.clear();
}

void displayTextOnTFT(String data, int posx, int posy, int fontType )
{
  switch(fontType)
  {
    case 0:
    tft.setFont(Terminal6x8);
    break;

    case 1:
    tft.setFont(Terminal11x16);
    break;

    case 2:
    tft.setFont(Terminal12x16);
    break;

    case 3:
    tft.setFont(Trebuchet_MS16x21);
    break;

    default:
    tft.setFont(Terminal6x8);  
    break;
  }
  tft.drawText(posx, posy, data);
}

void setup() 
{
  #if defined(ESP32)
  hspi.begin();
  tft.begin(hspi);
#else
  tft.begin();
#endif
  Serial.begin(112500);

  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {} // hold in infinite loop
  }

  radio.setAutoAck(false);

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1,linkerPipe); // using pipe 1, using the receiver ID address

  // start the listening pipe
  radio.startListening(); // put radio in RX mode

  // For debugging info
  printf_begin();             // needed only once for printing details
  radio.printDetails();       // (smaller) function that prints raw register values
  radio.printPrettyDetails(); // (larger) function that prints human readable data
}

void loop() 
{
  
  uint8_t pipe;
    if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
      radio.read(&payload, bytes);            // fetch payload from FIFO
      Serial.print(F("Received "));
      Serial.print(bytes);                    // print the size of the payload
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe);                     // print the pipe number
      Serial.print(F(": "));
      Serial.println(payload);                // print the payload's value

      String dataString = "BPM: " + String((int)payload);

      //clearDisplay();

      if(currentRSTValue == SESSION_RST_TRIGGER_VAL)
      {
        displayTextOnTFT("Session Reset...", 10, 200, 0);
        minBPM = (int) payload;
        currentRSTValue = 0;
      }
      else
      {
        displayTextOnTFT("                      ", 10, 200, 0);
      }

      // enter minBPM adjuster
      if(minBPM > (int) payload)
      {
        minBPM = (int) payload;
      }
      if(maxBPM < (int) payload)
      {
        maxBPM = (int) payload;
      }

      // Main display
      displayTextOnTFT("BPM METER", 10, 10, 2);
      displayTextOnTFT(dataString, 10, 35, 2);
      displayTextOnTFT("Max: " + String(maxBPM), 10, 55, 1);
      displayTextOnTFT("Min: " + String(minBPM), 10, 75, 1);

      currentRSTValue++; // increment the RST Trigger value
      
    }
    else
    {
      Serial.println("NO DATA RECEIVED!");
    }
    delay(1000);
}
