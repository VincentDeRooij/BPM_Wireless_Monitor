#include "MicroTimer.h"

MicroTimer::MicroTimer()
{
    this->microsTimer = time(nullptr); // start the timer
}

uint32_t MicroTimer::getElapsedMicros()
{
    // this function assumes that the timer was started using
    // `clock_gettime(CLOCK_MONOTONIC_RAW, &startTimer);`

    clock_gettime(CLOCK_MONOTONIC_RAW, &endTimer);
    uint32_t seconds = endTimer.tv_sec - startTimer.tv_sec;
    uint32_t useconds = (endTimer.tv_nsec - startTimer.tv_nsec) / 1000;

    return ((seconds)*1000 + useconds) + 0.5;
}