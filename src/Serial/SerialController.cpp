#include "SerialController.h"
#include <serial/serial.h>
#include <unistd.h> // UNIX SYSTEM SPECIFIC
#include <iostream>

#include <exception>

SerialController::SerialController()
{
    // set the port
    this->uartController.setPort(COM_PORT_ADDR);

    // set the baudrate
    this->uartController.setBaudrate(UART_BAUDRATE);

    // set the time-out
    serial::Timeout t_out = serial::Timeout::simpleTimeout(PORT_TIME_OUT); // set time-out
    this->uartController.setTimeout(t_out);
}

uint8_t* SerialController::ReadSerialBus()
{
    // open the port of the serial controller
    while (!this->uartController.isOpen())
    {
        this->uartController.open();

        usleep(1000 * 20); // sleep for 20 us * 1000 = 20ms
    }

    uint8_t msgData[10];

    try
    {
        // read serial message
        size_t msgSize = this->uartController.read((uint8_t *)msgData, (sizeof(uint8_t) * 10));

        if (msgSize == sizeof(msgData)) // if msg received is of correct size
        {
            return msgData;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Error Serial Message incorrect!" << std::endl;
    }
    return msgData; // if this is returned something went wrong, value returned = 0
}