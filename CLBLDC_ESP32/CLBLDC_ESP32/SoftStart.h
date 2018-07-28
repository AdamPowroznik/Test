#pragma once
//#include "InputOutput.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"

#endif
class SoftStart
{
	//int pwm;
	static int setPwm(int lastPwm, int futurePwm);
public:
	//SoftStart();
	//~SoftStart();
	static int GetPwmSoft(int lastPwm, int futurePwm);
};

