#include "InputOutput.h"

#include "ACS712.h"
#include "Voltometer.h"


InputOutput::InputOutput(int mainEnabledPin, int mainPotPin, int sidePin, ACS712 &currentMeter, Voltometer &voltometer)
{
	Serial.println("IO ports configuring...");

	this->mainEnabledPin = mainEnabledPin;
	this->mainPotPin = mainPotPin;
	this->sidePin = sidePin;

	pinMode(mainEnabledPin, INPUT_PULLUP);
	pinMode(mainPotPin, INPUT);
	pinMode(sidePin, INPUT_PULLUP);

	
	this->currentMeter = &currentMeter;
	this->voltometer = &voltometer;

	Serial.println("IO ports configured sucessfully.");
}

InputOutput::~InputOutput()
{
}

bool InputOutput::GetMainEnabled()
{
	setEnabled();
	return mainEnabled;
}

int InputOutput::GetNowTime()
{
	setNow();
	return now;
}

int InputOutput::GetMainPwm()
{
	setMainPwm();
	return mainPwm;
}

char InputOutput::GetSide()
{
	setSide();
	return side;
}

double InputOutput::GetRpm(volatile int timesCArray[], int numberOfElements)
{
	if(timesCArray[numberOfElements-1] > 0){
		setRpm(timesCArray, numberOfElements);
		return rpm;
	}
	else return 0;
}

double InputOutput::GetVolts(int samples)
{
	double vol = voltometer->GetVoltage(samples);
	//Serial.println(vol);
	return vol;
}


double InputOutput::GetAmps(int samples)
{
	double amp = currentMeter->getAmps(samples);
	//Serial.println(amp);
	return -amp;
}


void InputOutput::setEnabled() {
	if (digitalRead(mainEnabledPin) == LOW)
		mainEnabled = false;
	else mainEnabled = true;
}

void InputOutput::setNow()
{
	now = millis();
}

void InputOutput::setMainPwm()
{
	
	mainPwm = map(analogRead(mainPotPin), 0, 4095, 75, 255);
	//Serial.println(mainPwm);
}

void InputOutput::setSide()
{
	if (digitalRead(sidePin) == LOW) {
		side = 'L';
	}
	else {
		side = 'R';
	}
}

void InputOutput::setRpm(volatile int timesCArray[], int numberOfElements)
{
	long avTimeC = 0;
	for (int i = numberOfElements-1; i >= 0; i--)
	{
		avTimeC += timesCArray[i];
	}
	avTimeC /= numberOfElements;
	oneRotate = avTimeC * 3;
	rpm = 60000000 / oneRotate;
}

