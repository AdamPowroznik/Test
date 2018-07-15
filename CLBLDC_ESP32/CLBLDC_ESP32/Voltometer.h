#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif


class Voltometer
{
	int pin;
	double voltage;

public:
	Voltometer(int pin);
	Voltometer();
	~Voltometer();
	double SetVoltage(int samples);
	double GetVoltage(int samples);
	void SetPin(int pin);
};

