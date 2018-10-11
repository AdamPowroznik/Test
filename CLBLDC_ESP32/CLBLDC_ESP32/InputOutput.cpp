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

InputOutput::InputOutput()
{
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

int InputOutput::GetMainPwm(bool softStart)
{
	int lastPwm = this->mainPwm;
	setMainPwm();
	if (softStart)
		mainPwm = SoftStart::GetPwmSoft(lastPwm, mainPwm);
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

int InputOutput::GetWantedSpeed()
{
	setWantedSpeed();
	return wantedSpeed;
}

int InputOutput::GetWantedPwm(int wantedSpeed, int currentSpeed, int wantedPwm, bool softStart, int currPwm)
{
	setWantedPwm(wantedSpeed, currentSpeed, wantedPwm);
	if (softStart) {
		this->wantedPwm = SoftStart::GetPwmSoft(currPwm, this->wantedPwm);
	}
	return this->wantedPwm;
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

void InputOutput::setWantedSpeed()
{
	for (size_t i = 0; i < 20; i++)
	{
		wantedSpeed += map(analogRead(mainPotPin), 0, 4095, 0, 1600);
	}
	wantedSpeed /= 20;
	//Serial.println(mainPwm);
}

void InputOutput::setWantedPwm(int wantedSpeed, int currentSpeed, int wantedPwm)
{
	if (currentSpeed > 0) {
		if (wantedSpeed < currentSpeed) {
			if (wantedPwm > minPwm) {
				if (currentSpeed - wantedSpeed > 800 && wantedPwm > 150)
					this->wantedPwm -= 50;
				else if (currentSpeed - wantedSpeed > 600 && wantedPwm > 130)
					this->wantedPwm -= 30;
				else if (currentSpeed - wantedSpeed > 400 && wantedPwm > 120)
					this->wantedPwm -= 20;
				else if (currentSpeed - wantedSpeed > 200 && wantedPwm > 110)
					this->wantedPwm -= 10;
				else if (currentSpeed - wantedSpeed > 200 && wantedPwm > 105)
					this->wantedPwm -= 5;
				else if (currentSpeed - wantedSpeed > 200 && wantedPwm > 103)
					this->wantedPwm -= 3;
				else
					this->wantedPwm--;
			}
			else;
		}
		else if (wantedSpeed > currentSpeed) {
			if (wantedPwm < 255) {
				if (wantedSpeed - currentSpeed > 800 && wantedPwm < 205)
					this->wantedPwm += 50;
				else if (wantedSpeed - currentSpeed > 600 && wantedPwm < 225)
					this->wantedPwm += 30;
				else if (wantedSpeed - currentSpeed > 400 && wantedPwm < 235)
					this->wantedPwm += 20;
				else if (wantedSpeed - currentSpeed > 200 && wantedPwm < 245)
					this->wantedPwm += 10;
				else if (wantedSpeed - currentSpeed > 100 && wantedPwm < 250)
					this->wantedPwm += 5;
				else if (wantedSpeed - currentSpeed > 100 && wantedPwm < 252)
					this->wantedPwm += 3;
				else 
					this->wantedPwm++;
			}
		}
		//Serial.println(wantedSpeed / currentSpeed);
			
	}
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

