#pragma once
#include "MovementMode.h"
#include "InputOutput.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class BrakerMode :
	public MovementMode
{
public:
	InputOutput * IO;
	int printInterval = 2000, lastPrint;
	int lastPwm;

	int minPwm = 100;
	volatile bool LastWrittenPhaseA;
	volatile bool LastWrittenPhaseB;
	void IRAM_ATTR goLeftIRAM(bool phase1, bool phase2, int pwm);
	void IRAM_ATTR goRightIRAM(bool phase1, bool phase2, int pwm);
	void IRAM_ATTR Mode1PowerIRAM(bool phase1, bool phase2, int pwm, char side);

	void DONTMOVEINANYCASE();
	void TOLOWPWMSTOP();
	void TOLONGWITHOUTCHANGESTOP();
	void ResetArrays();
	void beginMoving();
	void stopMoving();
	void UpdateInputs();
	void UpdateOutputs();
	int getPwm();
	void changeSide();
	bool softStop();

public:
	int PWM;
	int RAWPWM;
	char SIDE;
	int NOW;
	bool MAINENABLED;
	double SPEED;
	double CURRENT;
	double VOLTAGE;

	int bonusPwmByPercent;
	int bonusPwmByValue;
	virtual void setParameter(bool trueup);

	void calcBonusPwmByPercent();

	BrakerMode(InputOutput &IO, int motorNPin, int motorSPin, int hallNPin, int hallSPin, int pwmCh1, int pwmCh2, int pwmFreq, int pwmRes);
	~BrakerMode();

	virtual void hallNIRQ();
	virtual void hallSIRQ();
	virtual void Work();
	virtual MovementTypes GetType();

	void PrintSomeValues();
	virtual void reset();
};

