#include "Voltometer.h"



Voltometer::Voltometer(int pin)
{
	this->pin = pin;
	pinMode(this->pin, INPUT_PULLDOWN);
	Serial.print("New Voltometer created on pin ");
	Serial.println(pin);
}

Voltometer::Voltometer()
{
	Serial.println("New Voltometer created without assigned pin.");
}


Voltometer::~Voltometer()
{
}

double Voltometer::SetVoltage(int samples) {
	double voltage;
 //Serial.print("  pin : ");
// Serial.print(pin);
//  Serial.print("  voltage : ");
//  Serial.print(voltage);
//  Serial.print("  samples : ");
// Serial.print(samples);
//  Serial.print("  analogRead : ");
//  Serial.print(analogRead(pin));
 // Serial.print("  map : ");
//  Serial.print(map(analogRead(pin), 0, 4095, 200, 3300));
	for (int i = 0; i<samples; i++)
	{
		voltage = voltage + map(analogRead(pin), 0, 4095, 200, 3300);
	}
 //Serial.print("  voltage2 : ");
 // Serial.print(voltage);
	voltage /= samples;
 voltage /= 100;
 //Serial.print("  voltage3 : ");
 // Serial.println(voltage);
	this->voltage = voltage;
	return voltage;
}

double Voltometer::GetVoltage(int samples) {
	 SetVoltage(samples);
	 return this->voltage;
}

void Voltometer::SetPin(int pin)
{
	this->pin = pin;
}
