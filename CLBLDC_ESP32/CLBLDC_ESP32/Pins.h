#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Pins
{
public:
	Pins();
	~Pins();

	static uint8_t PotPin;
	static uint8_t EnabledPin;
	static uint8_t SidePin;
	static uint8_t CurrentMeterPin;
	static uint8_t VoltageMeterPin;
	static uint8_t HallNPin;
	static uint8_t HallSPin;
	static uint8_t MotorNPin;
	static uint8_t MotorSPin;
	static uint8_t BuzzerPin;
	static uint8_t TemperatureMeterPin;
	static uint8_t Button1Pin;
	static uint8_t Button2Pin;
	static uint8_t Button3Pin;
	static uint8_t Button4Pin;
	static uint8_t GreenDiodePin;
	static uint8_t RedDiodePin;
	static uint8_t BlueDiodePin;
	static uint8_t LcdI2CSDA;
	static uint8_t LcdI2CSCL;
};

