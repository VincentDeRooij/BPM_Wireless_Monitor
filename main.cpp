#include <iostream>
#include <unistd.h>

#include "SerialController.h"

int main(int argc, char const *argv[])
{
    SerialController controller;

    uint8_t data;

    while (1)
    {
        data = controller.ReadSerialBus();

        std::cout << "DATA: " << data << std::endl;

        usleep(1000 * 2000); // repeat per 2 sec.
    }

    return 0;
}
