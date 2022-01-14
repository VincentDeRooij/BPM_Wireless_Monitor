#ifndef TIMER_H
#define TIMER_H
#pragma once

#include <stdint.h>
#include <time.h> // CLOCK_MONOTONIC_RAW, timespec, clock_gettime()
#include <ctime>  // time()

class MicroTimer
{
private:
	time_t microsTimer;
	struct timespec startTimer, endTimer;

public:
	uint32_t getElapsedMicros();

	MicroTimer();
	~MicroTimer() = default;
};
#endif