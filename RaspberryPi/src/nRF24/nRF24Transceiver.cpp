#include <iostream>

#include "nRF24Transceiver.h"
#include "MicroTimer.h"

MicroTimer timer;
RF24 transceiver(CE_PIN, 0);

const uint64_t linkerPipe = 0xF0F0F0F0D2LL;

bool setupTransceiver()
{
    std::cout << "MODULE SETTING UP" << std::endl;

    // start the transceiver
    if (!transceiver.begin())
    {
        std::cout << "nRF24 transceiver is not responding!" << std::endl;
        return 0; // quit now
    }

    // Fix to make sure it works (happens with alot of modules)
    transceiver.setAutoAck(false);

    std::cout << "MODULE ONLINE!" << std::endl;

    // setup the payload size
    transceiver.setPayloadSize(sizeof(int));

    // setup the PA level
    transceiver.setPALevel(PA_LEVEL);

    // setup the TX address of the RX node into the TX pipe
    transceiver.openWritingPipe(linkerPipe); // always uses pipe 0, using the transmitter ID address

    // setup the RX address of the TX node into a RX pipe
    //transceiver.openReadingPipe(1, pipes[1]); // using pipe 1, using the receiver ID address

    transceiver.printDetails();       // (smaller) function that prints raw register values
    transceiver.printPrettyDetails(); // (larger) function that prints human readable data
}

// Master type
void setToTransmitterType(float payload)
{
    transceiver.stopListening(); // put radio in TX mode

    unsigned int failure = 0; // keep track of failures
    while (failure < 6)
    {
        // clock_gettime(CLOCK_MONOTONIC_RAW, &startTimer);          // start the timer
        bool report = transceiver.write(&payload, sizeof(int)); // transmit & save the report
        uint32_t timerEllapsed = timer.getElapsedMicros();      // end the timer

        if (report)
        {
            // payload was delivered
            std::cout << "Transmission successful! Time to transmit = ";
            std::cout << timerEllapsed;                         // print the timer result
            std::cout << " us. Sent: " << payload << std::endl; // print payload sent
            payload += 0.01;                                    // increment float payload
        }
        else
        {
            // payload was not delivered
            std::cout << "Transmission failed or timed out" << std::endl;
            failure++;
        }

        // to make this example readable in the terminal
        delay(1000); // slow transmissions down by 1 second
    }
    std::cout << failure << " failures detected. Leaving TX role." << std::endl;
}

// Slave type
void setToReceiverType(int &payload)
{
    transceiver.startListening(); // put radio in RX mode

    time_t startTimer = time(nullptr); // start a timer
    while (time(nullptr) - startTimer < 6)
    { // use 6 second timeout
        uint8_t pipe;
        if (transceiver.available(&pipe))
        {                                                         // is there a payload? get the pipe number that recieved it
            uint8_t bytes = transceiver.getPayloadSize();         // get the size of the payload
            transceiver.read(&payload, bytes);                    // fetch payload from FIFO
            std::cout << "Received " << (unsigned int)bytes;      // print the size of the payload
            std::cout << " bytes on pipe " << (unsigned int)pipe; // print the pipe number
            std::cout << ": " << payload << std::endl;            // print the payload's value
            startTimer = time(nullptr);                           // reset timer
        }
    }
    std::cout << "Nothing received in 6 seconds. Leaving RX role." << std::endl;
    transceiver.stopListening();
}

// /**
//  * @brief Set the Role Of Transceiver object
//  *
//  * @param type Type of the transceiver
//  */
// void setRoleOfTransceiver(TRANCSEIVER_ROLE_TYPE type)
// {
//     switch (type)
//     {
//     case TRANCSEIVER_ROLE_TYPE::TRANSMITTER:
//         setToTransmitterType();
//         break;

//     case TRANCSEIVER_ROLE_TYPE::RECEIVER:
//         setToReceiverType();
//         break;

//     default:
//         std::cout << "Unknown type - cannot proceed" << std::endl;
//         break;
//     }
// }
