#pragma once
#include "MovementMode.h"
#include "InputOutput.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class ErrorHandler :
	public MovementMode
{
	//ERROR CODES:
	//0-current
	//1-temperature

	bool ERROR;

public:
	
	bool deletingError;
	InputOutput * IO;
	double maxCurrent = 2.9;
	int errorCode;
	String errorMessage1, errorMessage2;

	ErrorHandler(InputOutput &IO, int motorNPin, int motorSPin, int hallNPin, int hallSPin, int pwmCh1, int pwmCh2, int pwmFreq, int pwmRes);
	void Alarm();
	bool Monitor();
	bool GetStatus();
	virtual void hallNIRQ();
	virtual void hallSIRQ();
	virtual void Work();
	virtual MovementTypes GetType();
	virtual void setParameter(bool upordown);
	virtual void reset();
};

