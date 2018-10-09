#include "Pins.h"



Pins::Pins()
{
	EnabledPin = 15;
	PotPin = 27;
	SidePin = 13;
	CurrentMeterPin = 14;
	VoltageMeterPin = 12;
	HallNPin = 25;
	HallSPin = 26;
	MotorNPin = 33;
	MotorSPin = 32;
	BuzzerPin = 2;
	TemperatureMeterPin = 23;
	Button1Pin = 5;
	Button2Pin = 17;
	Button3Pin = 16;
	Button4Pin = 4;
	GreenDiodePin = 19;
	BlueDiodePin = 18;
	LcdI2CSDA = 21;
	LcdI2CSCL = 22;
}


Pins::~Pins()
{
}

