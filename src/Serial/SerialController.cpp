#include "SerialController.h"
#include <serial/serial.h>
#include <unistd.h> // UNIX SYSTEM SPECIFIC
#include <iostream>

#include <exception>

SerialController::SerialController()
{
    // set the port
    this->uartController.setPort(COM_PORT_ADDR);

    // ### Set Serial options ### //
    // Set the baudrate
    this->uartController.setBaudrate(UART_BAUDRATE);
    // Set Parity
    this->uartController.setParity(PARITY_STATE);
    // Set Bits msgsize
    this->uartController.setBytesize(BITS);
    // Set Stop Bits
    this->uartController.setStopbits(STOP_BITS);
    // Set Flow control
    this->uartController.setFlowcontrol(FLOW_CONTROL);

    // set the time-out
    serial::Timeout t_out = serial::Timeout::simpleTimeout(PORT_TIME_OUT); // set time-out
    this->uartController.setTimeout(t_out);
}

void SerialController::ReadSerialBus()
{
    // open the port of the serial controller
    while (!this->uartController.isOpen())
    {
        this->uartController.open();

        usleep(1000 * 20); // sleep for 20 us * 1000 = 20ms
    }

    std::cout << "Probing...." << std::endl;

    std::string size = std::to_string(100);

    auto msgData = this->uartController.read(size.length() + 1);

    std::cout << msgData << std::endl;

    // //uint8_t msgData[10];

    // try
    // {
    //     // read serial message

    //     return msgData;
    // }
    // catch (const std::exception &e)
    // {
    //     std::cout << "Error Serial Message incorrect!" << std::endl;
    // }
    // return msgData; // if this is returned something went wrong, value returned = 0
}
