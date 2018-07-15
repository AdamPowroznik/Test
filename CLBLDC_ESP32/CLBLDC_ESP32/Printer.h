#pragma once


#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "LiquidCrystal_I2C.h"
#include <Wire.h>

class Printer
{
	
	
public:
	int interval = 1000;
	LiquidCrystal_I2C * lcd;
	Printer(LiquidCrystal_I2C &lcd);
	~Printer();

	virtual void Print() = 0;
	void SetInterval(int value);
};

