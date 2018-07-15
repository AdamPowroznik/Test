#pragma once

#include "ACS712.h"
#include "Voltometer.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class InputOutput
{
	int current;
	int voltage;
	long rpm;
	int mainEnabled;
	int mainPwm;
	int now;
	char side;

	int currentMeterPin;
	int voltageMeterPin;
	int mainEnabledPin;
	int mainPotPin;
	int sidePin;

	unsigned long oneRotate;
	ACS712 *currentMeter;
	Voltometer *voltometer;

	void setEnabled();
	void setNow();
	void setMainPwm();
	void setSide();
	void setRpm(volatile int timesCArray[], int numberOfElements);
	

public:

	InputOutput(int mainEnabledPin, int mainPotPin, int sidePin, ACS712 &currentMeter, Voltometer &voltometer);
	~InputOutput();
	bool GetMainEnabled();
	int GetNowTime();
	int GetMainPwm();
	char GetSide();
	double GetRpm(volatile int timesCArray[], int numberOfElements);
	double GetVolts(int samples);
	double GetAmps(int samples);

};

