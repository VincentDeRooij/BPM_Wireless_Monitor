#ifndef NRF24TRANSCEIVER_H
#define NRF24TRANSCEIVER_H
#pragma once

#include <stdint.h>
#include "RF24/RF24.h"

// created with the help of
// >https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
// >https://create.arduino.cc/projecthub/muhammad-aqib/nrf24l01-interfacing-with-arduino-wireless-communication-0c13d4

#define CE_PIN 22 // GPIO pin on the Raspberry Pi
#define CSN_PIN 8 // CE GPIO pin on the Raspberry Pi
#define PAYLOAD_SIZE sizeof(int)
#define PA_LEVEL RF24_PA_LOW

enum TRANCSEIVER_ROLE_TYPE
{
	TRANSMITTER = 1,
	RECEIVER = 0
};

bool setupTransceiver();
void nRFWriteAndTransmit(float payload); // Master type
void nRFReceiveAndRead(float &payload);	 // Slave type
//void setRoleOfTransceiver(TRANCSEIVER_ROLE_TYPE type);

#endif // __NRF24TRANSCEIVER_H__
