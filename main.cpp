#include <iostream>
#include <unistd.h>

#include "SerialController.h"

#include <CppLinuxSerial/SerialPort.hpp>

using namespace mn::CppLinuxSerial;

int main(int argc, char const *argv[])
{

    // Create serial port object and open serial port at 57600 buad, 8 data bits, no parity bit, and one stop bit (8n1)
    SerialPort serialPort("/dev/ttyS0", BaudRate::B_9600, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE);
    // Use SerialPort serialPort("/dev/ttyACM0", 13000); instead if you want to provide a custom baud rate
    serialPort.SetTimeout(-1); // Block when reading until any data is received
    serialPort.Open();

    while (1)
    {

        // Write some ASCII data
        //serialPort.Write("Hello");

        // Read some data back (will block until at least 1 byte is received due to the SetTimeout(-1) call above)
        std::string readData;
        serialPort.Read(readData);
        std::cout << "DATA: " << readData << std::endl;

        usleep(1000 * 3000);
    }

    // Close the serial port
    serialPort.Close();

    // SerialController controller;

    // while (1)
    // {
    //     controller.ReadSerialBus();

    //     // memcpy(&formData, &data, sizeof(uint8_t) * 10);

    //     // std::cout << "DATA_t_0: " << formData[1] << std::endl;
    //     // std::cout << "DATA_t_1: " << unsigned(formData[1]) << std::endl;

    //     usleep(1000 * 1000); // repeat per 2 sec.
    // }

    // return 0;
}
