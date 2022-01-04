#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H
#pragma once

#include <serial/serial.h>
#include <stdint.h>

#define COM_PORT_ADDR "/dev/ttyS0" // uart port of the raspberry pi (tested on RPI v4 Model B)
#define PORT_TIME_OUT 1000

// Serial Options
#define UART_BAUDRATE 112500 // std baudrate (speed) of UART
#define PARITY_STATE serial::parity_none
#define BITS serial::bytesize_t::eightbits
#define STOP_BITS serial::stopbits_t::stopbits_one
#define FLOW_CONTROL serial::flowcontrol_t::flowcontrol_none

/**
 * @brief Class for the HC-12 communication module 
 * 
 */
class SerialController
{
private:
	serial::Serial uartController;

public:
	void ReadSerialBus();

	SerialController();
	~SerialController() = default;
};
#endif