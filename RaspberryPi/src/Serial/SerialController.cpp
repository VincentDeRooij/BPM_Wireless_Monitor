#include "SerialController.h"
//#include <serial/serial.h>
#include <unistd.h> // UNIX SYSTEM SPECIFIC
#include <iostream>

#include <exception>

struct SerialControllerDataPack
{
    int data;
};

SerialController::SerialController()
{
    // set the port
    this->uartController.SetDevice(COM_PORT_ADDR);

    // ### Set Serial options ### //
    // Set the baudrate
    this->uartController.SetBaudRate(UART_BAUDRATE);
    // Set Parity
    this->uartController.SetParity(PARITY_STATE);
    // Set Bits msgsize
    this->uartController.SetNumDataBits(BITS);
    // Set Stop Bits
    this->uartController.SetNumStopBits(STOP_BITS);
    // Set Flow control
    this->uartController.SetTimeout(PORT_TIME_OUT);
}

void SerialController::ReadSerialBus()
{
    // open the port of the serial controller
    this->uartController.Open();

    std::cout << "Probing...." << std::endl;

    std::string size = std::to_string(100);

    SerialControllerDataPack pack;

    std::cout << pack.data << std::endl;

    std::string readData;
    this->uartController.Read(readData);
}
