#include <iostream>
#include <unistd.h>

// string to <type> conversion
#include <string>
#include <sstream>

#include <CppLinuxSerial/SerialPort.hpp>

#include "SerialController.h"

#include "nRF24Transceiver.h"

bool nRF24IsNotSetUp = true;
using namespace mn::CppLinuxSerial;
bool nRFIsAvailable = false;

int main(int argc, char const *argv[])
{
    // Debug message for testing
    std::cout << "STARTING PROGRAM" << std::endl;

    // Create serial port object and open serial port at 57600 buad, 8 data bits, no parity bit, and one stop bit (8n1)
    SerialPort serialPort("/dev/ttyS0", BaudRate::B_9600, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE);
    serialPort.SetTimeout(-1); // Block when reading until any data is received
    serialPort.Open();         // open the serial port connection
    std::cout << "SERIAL PORT OPEN!" << std::endl;

    if (nRF24IsNotSetUp)
    {
        nRF24IsNotSetUp = false;
        std::cout << "NRF24 MODULE SETTING UP" << std::endl;
        setupTransceiver();
        std::cout << "NRF24 MODULE AVAILABLE!" << std::endl;
        nRFIsAvailable = true;
    }

    // initialize the values
    float data = 0.0;
    std::string readData = "";

    // enter the while-loop to constantly receive (from Arduino) and send data to the ESP
    while (true)
    {        
        std::cout << std::endl; // add a new line to make it more readable in the console

        std::cout << "------------------------UART RECEIVER-----------------------" << std::endl;
        // Read UART/Serial port
        serialPort.Read(readData);
        std::cout << "UART: Data received: " << readData << std::endl;

        std::cout << "------------------------NRF24 TRANSMITTER---------------------" << std::endl;
        // if nRF24 module is active and ready for use
        if (nRFIsAvailable == true && readData.length() != 0)
        {
	    std::cout << "NRF24: Sending data...." << std::endl;
            // convert the value needed to the propper type
            std::istringstream strValue(readData);
            strValue >> data;
            nRFWriteAndTransmit(data);
        }

        // sleep for two seconds
        usleep(1000 * 2000);
    }
    // Close the serial port
    serialPort.Close();
    return 0;
}
