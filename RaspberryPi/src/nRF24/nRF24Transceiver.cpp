#include <iostream>

#include "nRF24Transceiver.h"
#include "MicroTimer.h"

MicroTimer timer;
RF24 transceiver(CE_PIN, CSN_PIN);

uint8_t transmitterIDAddress[6] = {"MASTR"};
uint8_t receiverIDAddress[6] = {"SLAVE"};
int dataPayload = 0;

bool setupTransceiver()
{
    // start the transceiver
    if (!transceiver.begin())
    {
        cout << "nRF24 transceiver is not responding!" << endl;
        return 0; // quit now
    }

    // setup the payload size
    transceiver.setPayloadSize(PAYLOAD_SIZE);

    // setup the PA level
    transceiver.setPALevel(PA_LEVEL);

    // setup the TX address of the RX node into the TX pipe
    transceiver.openWritingPipe(transmitterIDAddress); // always uses pipe 0, using the transmitter ID address

    // setup the RX address of the TX node into a RX pipe
    transceiver.openReadingPipe(1, receiverIDAddress); // using pipe 1, using the receiver ID address
}

// Master type
void setToTransmitterType()
{
    transceiver.stopListening(); // put radio in TX mode

    unsigned int failure = 0; // keep track of failures
    while (failure < 6)
    {
        // clock_gettime(CLOCK_MONOTONIC_RAW, &startTimer);          // start the timer
        bool report = transceiver.write(&payload, sizeof(PAYLOAD_SIZE)); // transmit & save the report
        uint32_t timerEllapsed = timer.getElapsedMicros();               // end the timer

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

// Slave type
void setToReceiverType(int &payload)
{
    transceiver.startListening(); // put radio in RX mode

    time_t startTimer = time(nullptr); // start a timer
    while (time(nullptr) - startTimer < 6)
    { // use 6 second timeout
        uint8_t pipe;
        if (transceiver.available(&pipe))
        {                                                    // is there a payload? get the pipe number that recieved it
            uint8_t bytes = transceiver.getPayloadSize();    // get the size of the payload
            transceiver.read(&payload, bytes);               // fetch payload from FIFO
            cout << "Received " << (unsigned int)bytes;      // print the size of the payload
            cout << " bytes on pipe " << (unsigned int)pipe; // print the pipe number
            cout << ": " << payload << endl;                 // print the payload's value
            startTimer = time(nullptr);                      // reset timer
        }
    }
    cout << "Nothing received in 6 seconds. Leaving RX role." << endl;
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