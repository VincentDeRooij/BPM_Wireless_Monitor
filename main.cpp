#include <iostream>
#include <unistd.h>

#include "SerialController.h"

int main(int argc, char const *argv[])
{
    SerialController controller;

    uint8_t *data;
    uint8_t formData[10];

    while (1)
    {
        controller.ReadSerialBus();

        // memcpy(&formData, &data, sizeof(uint8_t) * 10);

        // std::cout << "DATA_t_0: " << formData[1] << std::endl;
        // std::cout << "DATA_t_1: " << unsigned(formData[1]) << std::endl;

        // usleep(1000 * 2000); // repeat per 2 sec.
    }

    return 0;
}
