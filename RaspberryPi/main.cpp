// #include <iostream>
// #include <unistd.h>

// #include <CppLinuxSerial/SerialPort.hpp>

// #include "SerialController.h"

// #include "nRF24Transceiver.h"

// bool nRF24IsActive = true;

// using namespace mn::CppLinuxSerial;

// int main(int argc, char const *argv[])
// {
//     std::cout << "STARTING PROGRAM" << std::endl;

//     // Create serial port object and open serial port at 57600 buad, 8 data bits, no parity bit, and one stop bit (8n1)
//     SerialPort serialPort("/dev/ttyS0", BaudRate::B_9600, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE);
//     // Use SerialPort serialPort("/dev/ttyACM0", 13000); instead if you want to provide a custom baud rate
//     serialPort.SetTimeout(-1); // Block when reading until any data is received
//     serialPort.Open();

//     std::cout << "SERIAL PORT OPEN!" << std::endl;

//     if (nRF24IsActive)
//     {
// 	std::cout << "NRF24 MODULE SETTING UP" << std::endl;

//         setupTransceiver();
//         int data = 100;
//         setToTransmitterType(data);
//         //setRoleOfTransceiver(TRANCSEIVER_ROLE_TYPE::TRANSMITTER);
//     }

//     while (1)
//     {
//         // Write some ASCII data
//         //serialPort.Write("Hello");

//         // Read some data back (will block until at least 1 byte is received due to the SetTimeout(-1) call above)
//         std::string readData;
//         serialPort.Read(readData);
//         std::cout << "DATA: " << readData << std::endl;

//         usleep(1000 * 2000);

//         if (nRF24IsActive)
//         {
//         }
//     }
//     // Close the serial port
//     serialPort.Close();

//     // SerialController controller;

//     // while (1)
//     // {
//     //     controller.ReadSerialBus();

//     //     // memcpy(&formData, &data, sizeof(uint8_t) * 10);

//     //     // std::cout << "DATA_t_0: " << formData[1] << std::endl;
//     //     // std::cout << "DATA_t_1: " << unsigned(formData[1]) << std::endl;

//     //     usleep(1000 * 1000); // repeat per 2 sec.
//     // }

//     // return 0;
// }

/*
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Brendan Doherty (2bndy5)
 */

/**
 * A simple example of sending data from 1 nRF24L01 transceiver to another.
 *
 * This example was written to be used on 2 devices acting as "nodes".
 * Use `ctrl+c` to quit at any time.
 */
#include <ctime>       // time()
#include <iostream>    // cin, cout, endl
#include <string>      // string, getline()
#include <time.h>      // CLOCK_MONOTONIC_RAW, timespec, clock_gettime()
#include <RF24/RF24.h> // RF24, RF24_PA_LOW, delay()

using namespace std;

/****************** Linux ***********************/
// Radio CE Pin, CSN Pin, SPI Speed
// CE Pin uses GPIO number with BCM and SPIDEV drivers, other platforms use their own pin numbering
// CS Pin addresses the SPI bus number at /dev/spidev<a>.<b>
// ie: RF24 radio(<ce_pin>, <a>*10+<b>); spidev1.0 is 10, spidev1.1 is 11 etc..

// Generic:
RF24 radio(22, 0);
/****************** Linux (BBB,x86,etc) ***********************/
// See http://nRF24.github.io/RF24/pages.html for more information on usage
// See http://iotdk.intel.com/docs/master/mraa/ for more information on MRAA
// See https://www.kernel.org/doc/Documentation/spi/spidev for more information on SPIDEV

// For this example, we'll be using a payload containing
// a single float number that will be incremented
// on every successful transmission
float payload = 0.0;

void setRole(); // prototype to set the node's role
void master();  // prototype of the TX node's behavior
void slave();   // prototype of the RX node's behavior

// custom defined timer for evaluating transmission time in microseconds
struct timespec startTimer, endTimer;
uint32_t getMicros(); // prototype to get ellapsed time in microseconds

int main(int argc, char **argv)
{

    // perform hardware check
    if (!radio.begin())
    {
        cout << "radio hardware is not responding!!" << endl;
        return 0; // quit now
    }

    radio.setAutoAck(false);

    // to use different addresses on a pair of radios, we need a variable to
    // uniquely identify which address this radio will use to transmit
    bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit

    // print example's name
    cout << argv[0] << endl;

    // Let these addresses be used for the pair
    const uint64_t pipes[2] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL}; // It is very helpful to think of an address as a path instead of as
    // It is very helpful to think of an address as a path instead of as
    // an identifying device destination

    // Set the radioNumber via the terminal on startup
    cout << "Which radio is this? Enter '0' or '1'. Defaults to '0' ";
    string input;
    getline(cin, input);
    radioNumber = input.length() > 0 && (uint8_t)input[0] == 49;

    // save on transmission time by setting the radio to only transmit the
    // number of bytes we need to transmit a float
    radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes

    // Set the PA Level low to try preventing power supply related problems
    // because these examples are likely run with nodes in close proximity to
    // each other.
    radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.

    // set the TX address of the RX node into the TX pipe
    // set the TX address of the RX node into the TX pipe
    radio.openWritingPipe(pipes[0]); // always uses pipe 0

    // set the RX address of the TX node into a RX pipe
    radio.openReadingPipe(1, pipes[1]); // using pipe 1, using the receiver ID address

    // For debugging info
    radio.printDetails();       // (smaller) function that prints raw register values
    radio.printPrettyDetails(); // (larger) function that prints human readable data

    // ready to execute program now
    setRole(); // calls master() or slave() based on user input
    return 0;
}

/**
 * set this node's role from stdin stream.
 * this only considers the first char as input.
 */
void setRole()
{
    string input = "";
    while (!input.length())
    {
        cout << "*** PRESS 'T' to begin transmitting to the other node\n";
        cout << "*** PRESS 'R' to begin receiving from the other node\n";
        cout << "*** PRESS 'Q' to exit" << endl;
        getline(cin, input);
        if (input.length() >= 1)
        {
            if (input[0] == 'T' || input[0] == 't')
                master();
            else if (input[0] == 'R' || input[0] == 'r')
                slave();
            else if (input[0] == 'Q' || input[0] == 'q')
                break;
            else
                cout << input[0] << " is an invalid input. Please try again." << endl;
        }
        input = ""; // stay in the while loop
    }               // while
} // setRole()

/**
 * make this node act as the transmitter
 */
void master()
{
    radio.stopListening(); // put radio in TX mode

    unsigned int failure = 0; // keep track of failures
    while (failure < 6)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &startTimer);    // start the timer
        bool report = radio.write(&payload, sizeof(float)); // transmit & save the report
        uint32_t timerEllapsed = getMicros();               // end the timer

        if (report)
        {
            // payload was delivered
            cout << "Transmission successful! Time to transmit = ";
            cout << timerEllapsed;                    // print the timer result
            cout << " us. Sent: " << payload << endl; // print payload sent
            payload += 0.01;                          // increment float payload
        }
        else
        {
            // payload was not delivered
            cout << "Transmission failed or timed out" << endl;
            failure++;
        }

        // to make this example readable in the terminal
        delay(1000); // slow transmissions down by 1 second
    }
    cout << failure << " failures detected. Leaving TX role." << endl;
}

/**
 * make this node act as the receiver
 */
void slave()
{

    radio.startListening(); // put radio in RX mode

    time_t startTimer = time(nullptr); // start a timer
    while (time(nullptr) - startTimer < 6)
    { // use 6 second timeout
        uint8_t pipe;
        if (radio.available(&pipe))
        {                                                    // is there a payload? get the pipe number that recieved it
            uint8_t bytes = radio.getPayloadSize();          // get the size of the payload
            radio.read(&payload, bytes);                     // fetch payload from FIFO
            cout << "Received " << (unsigned int)bytes;      // print the size of the payload
            cout << " bytes on pipe " << (unsigned int)pipe; // print the pipe number
            cout << ": " << payload << endl;                 // print the payload's value
            startTimer = time(nullptr);                      // reset timer
        }
    }
    cout << "Nothing received in 6 seconds. Leaving RX role." << endl;
    radio.stopListening();
}

/**
 * Calculate the ellapsed time in microseconds
 */
uint32_t getMicros()
{
    // this function assumes that the timer was started using
    // `clock_gettime(CLOCK_MONOTONIC_RAW, &startTimer);`

    clock_gettime(CLOCK_MONOTONIC_RAW, &endTimer);
    uint32_t seconds = endTimer.tv_sec - startTimer.tv_sec;
    uint32_t useconds = (endTimer.tv_nsec - startTimer.tv_nsec) / 1000;

    return ((seconds)*1000 + useconds) + 0.5;
}
