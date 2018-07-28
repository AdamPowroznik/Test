#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif


enum MovementTypes {
	Momentum = 0,
	Speed = 1,

};



class MovementMode
{
public:
	int motorNPin;
	int motorSPin;
	int hallNPin;
	int hallSPin;
	int pwmCh1;
	int pwmCh2;
	int pwmFreq;
	int pwmRes;

	virtual void setParameter(bool upordown) = 0;

	volatile int TimeA, timeA2, TimeB, timeB2, timeC, TimeC;

	volatile int timesAArray[10];
	volatile int timesBArray[10];
	volatile int timesCArray[10];

	volatile int RtimesAArray[9];
	volatile int RtimesBArray[9];
	volatile int RtimesCArray[9];

	volatile bool SpoleWasLast;
	volatile bool NpoleWasLast;
	volatile int changesCounter;

	void IRAM_ATTR updateArrayC();
	void IRAM_ATTR getTimeC();
	void IRAM_ATTR updateArrayB(int lastB);
	void IRAM_ATTR updateArrayA(int lastA);

//public:
	MovementMode(int motorNPin, int motorSPin, int hallNPin, int hallSPin, int pwmCh1, int pwmCh2, int pwmFreq, int pwmRes);
	void pwmSetup();
	void Begin();
	virtual void hallNIRQ();
	virtual void hallSIRQ();
	virtual void Work() = 0;
	virtual MovementTypes GetType() = 0;
	
};

