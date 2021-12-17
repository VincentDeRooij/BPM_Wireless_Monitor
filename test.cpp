#include <iostream>
#include <unistd.h>
#include "MAX30102.h"
#include <sys/time.h>

using namespace std;

MAX30102 heartSensor;

// Library from https://github.com/garrettluu/max30102-rpi

int main(void)
{
	cout << "MAX30102 Heart Sensor Tester" << endl
		 << "-----------------------------------------" << endl;
	// Start i2c
	int result = heartSensor.begin();
	if (result < 0)
	{
		cout << "Failed to start I2C (Error: " << result << ")." << endl;
		return (-1 * result);
	}
	cout << "Device found (revision: " << result << ")!" << endl;

	heartSensor.setup();
	//heartSensor.softReset();
	heartSensor.setPulseAmplitudeRed(0x0A);
	struct timeval time_now{};

	long lastTime = 0;
	long deltaTime = 0;
	float bpm = 0;

	while (1)
	{
                //struct timeval time_now{};
	    	gettimeofday(&time_now, nullptr);
    		time_t millisecs = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
		long irValue = heartSensor.getIR();

		std::cout << "MILIS" << millisecs << std::endl;

		if(irValue > 100000)
		{
			//
			deltaTime = millisecs - lastTime;
			lastTime = deltaTime;

			bpm = 60 / (deltaTime / 1000.0);

			std::cout << "BPM: " << bpm << std::endl;
		}



		//cout << "Temperature: " << heartSensor.readTemperatureF() << endl;
		cout << "IR: " << heartSensor.getIR();
		cout << ", RED: " << heartSensor.getRed();
		cout << endl;
		//usleep(500 * (1000)); // sleep for 500ms
		//break;
	}
	//heartSensor.shutDown();

	return 0;
}
