#pragma once
#include "MovementMode.h"
#include "InputOutput.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class StaticMomentum :
	public MovementMode
{
	//portMUX_TYPE *mux;
	InputOutput *IO;
	int printInterval = 200, lastPrint;
	int lastPwm;
	
	int minPwm = 100;
	bool softStopping;
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

	void DONTMOVEINANYCASE();
	void TOLOWPWMSTOP();
	void TOLONGWITHOUTCHANGESTOP();
	void RETRYMOVING();
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
	volatile bool FIRSTRUN;
	int NOW;
	bool MAINENABLED;
	double SPEED;
	double CURRENT;
	double VOLTAGE;

	bool softStart = true;

	int bonusPwmByPercent;
	int bonusPwmByValue;
	virtual void setParameter(bool trueup);

	void calcBonusPwmByPercent();

	StaticMomentum(InputOutput &IO, int motorNPin, int motorSPin, int hallNPin, int hallSPin, int pwmCh1, int pwmCh2, int pwmFreq, int pwmRes);
	~StaticMomentum();

	virtual void hallNIRQ();
	virtual void hallSIRQ();
	virtual void Work();
	virtual MovementTypes GetType();
	virtual void reset();
	void PrintSomeValues();
};

