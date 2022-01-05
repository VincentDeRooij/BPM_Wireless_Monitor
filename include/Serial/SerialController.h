#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H
#pragma once

//#include <serial/serial.h>
#include <CppLinuxSerial/SerialPort.hpp>
#include <stdint.h>

#define COM_PORT_ADDR "/dev/ttyS0" // uart port of the raspberry pi (tested on RPI v4 Model B)
#define PORT_TIME_OUT -1

// Serial Options
#define UART_BAUDRATE mn::CppLinuxSerial::BaudRate::B_9600 // std baudrate (speed) of UART
#define PARITY_STATE mn::CppLinuxSerial::Parity::NONE
#define BITS mn::CppLinuxSerial::NumDataBits::EIGHT
#define STOP_BITS mn::CppLinuxSerial::NumStopBits::ONE

/**
 * @brief Class for the HC-12 communication module 
 * 
 */
class SerialController
{
private:
	mn::CppLinuxSerial::SerialPort uartController;

public:
	void ReadSerialBus();

	SerialController();
	~SerialController() = default;
};
#endif
