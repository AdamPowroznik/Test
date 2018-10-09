#include "MovementMode.h"


void IRAM_ATTR MovementMode::updateArrayC()
{
	for (int i = 0; i <= 8; i++) {
		timesCArray[i] = timesCArray[i + 1];
	}
	timesCArray[9] = TimeC;
}

void IRAM_ATTR MovementMode::getTimeC()
{
	TimeC = micros() - timeC;
	timeC = micros();
	updateArrayC();
}

void IRAM_ATTR MovementMode::updateArrayB(int lastB)
{
	for (int i = 0; i <= 8; i++) {
		timesBArray[i] = timesBArray[i + 1];
	}
	timesBArray[9] = lastB;
}

void IRAM_ATTR MovementMode::updateArrayA(int lastA)
{
	for (int i = 0; i <= 8; i++) {
		timesAArray[i] = timesAArray[i + 1];
	}
	timesAArray[9] = lastA;
}

MovementMode::MovementMode(int motorNPin, int motorSPin, int hallNPin, int hallSPin, int pwmCh1, int pwmCh2, int pwmFreq, int pwmRes)
{
		this->motorNPin = motorNPin;
		this->motorSPin = motorSPin;
		this->hallNPin = hallNPin;	
		this->hallSPin = hallSPin;
		this->pwmCh1 = pwmCh1;
		this->pwmCh2 = pwmCh2;
		this->pwmFreq = pwmFreq;
		this->pwmRes = pwmRes;
}

void MovementMode::pwmSetup()
{
		ledcSetup(pwmCh1, pwmFreq, pwmRes);
		ledcAttachPin(motorNPin, pwmCh1);
		ledcSetup(pwmCh2, pwmFreq, pwmRes);
		ledcAttachPin(motorSPin, pwmCh2);
		Serial.println("Pwm channels configured sucessfully.");
}

void MovementMode::Begin()
{
	pinMode(this->motorNPin, OUTPUT);
	pinMode(this->motorSPin, OUTPUT);
	pinMode(this->hallNPin, INPUT);
	pinMode(this->hallSPin, INPUT);

	Serial.println("New movement pins assigned: ");
	Serial.print("	Motor N pin -> ");
	Serial.println(motorNPin);
	Serial.print("	Motor S pin -> ");
	Serial.println(motorSPin);
	Serial.print("	Hall N pin -> ");
	Serial.println(hallNPin);
	Serial.print("	Hall S pin -> ");
	Serial.println(hallSPin);
	pwmSetup();
}

void MovementMode::hallNIRQ()
{
	if (SpoleWasLast) {
		changesCounter++;
		timeA2 = micros();
		TimeB = micros() - timeB2;
		updateArrayB(TimeB);
		getTimeC();
	}
	NpoleWasLast = true;
	SpoleWasLast = false;
}

void MovementMode::hallSIRQ()
{
	if (NpoleWasLast) {
		changesCounter++;
		timeB2 = micros();
		TimeA = micros() - timeA2;
		updateArrayA(TimeA);
	}
	SpoleWasLast = true;
	NpoleWasLast = false;
}

