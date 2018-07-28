#pragma once

#include "ACS712.h"
#include "Voltometer.h"
#include "SoftStart.h"

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
	int wantedSpeed;
	int wantedPwm=100;

	int minPwm = 100;

	int currentMeterPin;
	int voltageMeterPin;
	int mainEnabledPin;
	int mainPotPin;
	int sidePin;

	ACS712 *currentMeter;
	Voltometer *voltometer;

	void setEnabled();
	void setNow();
	void setMainPwm();
	void setSide();
	void setRpm(volatile int timesCArray[], int numberOfElements);
	void setWantedSpeed();
	void setWantedPwm(int wantedSpeed, int currentSpeed, int wantedPwm);

public:

	unsigned long oneRotate;
	InputOutput(int mainEnabledPin, int mainPotPin, int sidePin, ACS712 &currentMeter, Voltometer &voltometer);
	InputOutput();
	~InputOutput();
	bool GetMainEnabled();
	int GetNowTime();
	int GetMainPwm(bool softStart);
	char GetSide();
	double GetRpm(volatile int timesCArray[], int numberOfElements);
	double GetVolts(int samples);
	double GetAmps(int samples);
	int GetWantedSpeed();
	int GetWantedPwm(int wantedSpeed, int currentSpeed, int wantedPwm, bool softStart);
};

