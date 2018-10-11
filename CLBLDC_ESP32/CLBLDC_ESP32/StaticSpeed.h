#pragma once
#include "MovementMode.h"
#include "InputOutput.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class StaticSpeed :
	public MovementMode
{
	bool sidechanging;
public:
	int bonusSpeedByValue;
	void changeSide();
	bool softStop();
	InputOutput * IO;
	int printInterval = 2000, lastPrint;
	int minPwm = 100;
	int lastPwm;
	int oneRotateCounter;
	int fakeIRQcounter;
	volatile int lastPolyChange;
	int lastFakeTime;
	volatile int timeSinceLastChange;
	volatile bool LastWrittenPhaseA;
	volatile bool LastWrittenPhaseB;
	void IRAM_ATTR goLeftIRAM(bool phase1, bool phase2, int pwm);
	void IRAM_ATTR goRightIRAM(bool phase1, bool phase2, int pwm);
	void IRAM_ATTR Mode1PowerIRAM(bool phase1, bool phase2, int pwm, char side);
	void IRAM_ATTR makeFakeIRQ();
	void IRAM_ATTR makeFakeIRQ2();
	void calcBonusPwmByPercent();
	void DONTMOVEINANYCASE();
	void TOLOWPWMSTOP();
	void TOLONGWITHOUTCHANGESTOP();
	void RETRYMOVING();
	void ResetArrays();
	void beginMoving();
	void stopMoving();
	void UpdateInputs();

	int bonusSpeedByValue2;
public:
	
	bool softStart = 0;
	int WANTEDSPEED;
	int NORMALPWM;
	int WANTEDPWM=100;
	char SIDE;
	volatile bool FIRSTRUN = 1;
	int NOW;
	bool MAINENABLED;
	double SPEED;
	double CURRENT;
	double VOLTAGE;
	StaticSpeed(InputOutput &IO, int motorNPin, int motorSPin, int hallNPin, int hallSPin, int pwmCh1, int pwmCh2, int pwmFreq, int pwmRes);
	~StaticSpeed();
	virtual void hallNIRQ();
	virtual void hallSIRQ();
	virtual void Work();
	virtual MovementTypes GetType();
	//	void printToLcd();
	void PrintSomeValues();

	//int bonusPwmByPercent;
	//int bonusPwmByValue;
	virtual void setParameter(bool trueup);
	//void calcBonusPwmByPercent();
	virtual void reset();
};

