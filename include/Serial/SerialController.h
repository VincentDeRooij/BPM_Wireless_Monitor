#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H
#pragma once

#include <serial/serial.h>
#include <stdint.h>

#define UART_BAUDRATE 9600			 // std baudrate (speed) of UART
#define COM_PORT_ADDR "/dev/ttyAMA0" // uart port of the raspberry pi (tested on RPI v4 Model B)
#define PORT_TIME_OUT 2500

/**
 * @brief Class for the HC-12 communication module 
 * 
 */
class SerialController
{
private:
	serial::Serial uartController;

public:
	uint8_t *ReadSerialBus();

	SerialController();
	~SerialController() = default;
};
#endif