#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include "MAX30102.h"
#include <sys/time.h>
#include <stdio.h>

using namespace std;

MAX30102 heartSensor;

// Library from https://github.com/garrettluu/max30102-rpi

struct timeval __millis_start;

void init_millis() {
    gettimeofday(&__millis_start, NULL);
};

unsigned long int millis() {
    long mtime, seconds, useconds; 
    struct timeval end;
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - __millis_start.tv_sec;
    useconds = end.tv_usec - __millis_start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    return mtime;
};

long lastBeat = 0;


int main(void)
{	
	init_millis();
	while (1)
	{
		long delta = millis() - lastBeat;
		
    	printf("Elapsed time: %ld milliseconds\n", millis());		
	}
}
