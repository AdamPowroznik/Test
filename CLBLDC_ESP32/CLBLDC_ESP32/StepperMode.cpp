#include "StepperMode.h"



void StepperMode::setParameter(bool trueup)
{
	if (trueup) {
		stepsToDo++;
	}
	else {
		stepsToDo--;
	}
}

void StepperMode::calcBonusPwmByPercent()
{
	bonusPwmByValue = (bonusPwmByPercent * 255) / 100;
	/*Serial.print(bonusPwmByPercent);
	Serial.print("		");
	Serial.println(bonusPwmByValue);*/
}

StepperMode::StepperMode(InputOutput &IO, int motorNPin, int motorSPin, int hallNPin, int hallSPin, int pwmCh1, int pwmCh2, int pwmFreq, int pwmRes)
	:MovementMode(motorNPin, motorSPin, hallNPin, hallSPin, pwmCh1, pwmCh2, pwmFreq, pwmRes)
{
	this->IO = &IO;
	UpdateInputs();
	Serial.println("Static Momentum Mode configurated sucessfully.");
}


StepperMode::~StepperMode()
{
}

void StepperMode::makeOneStep()
{
	makeFakeIRQ();
}

void StepperMode::hallNIRQ()
{
	MovementMode::hallNIRQ();
}

void StepperMode::hallSIRQ()
{
	MovementMode::hallSIRQ();
}

void StepperMode::Work()
{
	static int lastStepTime;
	UpdateInputs();
	if (MAINENABLED)
	{
		UpdateOutputs();
		digitalWrite(19, 1);
		timeSinceLastChange = NOW - lastPolyChange;
		Serial.println(stepsToDo);
		if (RAWPWM >= minPwm)
		{
			if ((NOW - lastStepTime > 3000) && stepsToDo > 0) {
				lastStepTime = millis();
				makeOneStep();
				makeOneStep();
				delay(300);
				stepsToDo--;
			}
			Mode1PowerIRAM(true, false, 100, SIDE);
			
			
		}
		else if (RAWPWM <= minPwm - 20)
		{
			TOLOWPWMSTOP();
		}
	}
	else
	{
		DONTMOVEINANYCASE();
	}

	if (lastPrint + printInterval < NOW) {
		lastPrint = millis();
		PrintSomeValues();
		//printToLcd();
	}
}

MovementTypes StepperMode::GetType()
{
	return Stepper;
}

void IRAM_ATTR StepperMode::goLeftIRAM(bool phase1, bool phase2, int pwm)
{
	lastPolyChange = millis();
	if (phase1) {
		ledcWrite(pwmCh1, 0);
		ledcWrite(pwmCh2, pwm);
	}
	else if (phase2) {
		ledcWrite(pwmCh2, 0);
		ledcWrite(pwmCh1, pwm);
	}
}

void IRAM_ATTR StepperMode::goRightIRAM(bool phase1, bool phase2, int pwm)
{
	lastPolyChange = millis();
	if (phase1) {
		ledcWrite(pwmCh2, 0);
		ledcWrite(pwmCh1, pwm);
	}
	else if (phase2) {
		ledcWrite(pwmCh1, 0);
		ledcWrite(pwmCh2, pwm);
	}
}

void IRAM_ATTR StepperMode::Mode1PowerIRAM(bool phase1, bool phase2, int pwm, char side)
{
	lastPwm = pwm;
	LastWrittenPhaseA = phase1;
	LastWrittenPhaseB = phase2;
	if (side == 'L') {
		goLeftIRAM(phase1, phase2, pwm);
	}
	else if (side == 'R') {
		goRightIRAM(phase1, phase2, pwm);
	}
}


void StepperMode::DONTMOVEINANYCASE()
{
	TOLOWPWMSTOP();
	digitalWrite(19, 0);
}

void StepperMode::TOLOWPWMSTOP()
{
	FIRSTRUN = true;
	stopMoving();
	changesCounter = 0;
	ResetArrays();
	fakeIRQcounter = 0;
	lastFakeTime = 0;
	timeSinceLastChange = -1;
	lastPolyChange = 0;
	PWM = 0;
}

void StepperMode::TOLONGWITHOUTCHANGESTOP()
{
	ResetArrays();
	changesCounter = 0;
	timeSinceLastChange = -1;//NOT SURE ABOUT THOSE TWO
	lastPolyChange = 0;//NOT SURE ABOUT THOSE TWO
}

void StepperMode::ResetArrays()
{
	for (int i = 0; i <10; i++) {
		timesCArray[i] = 0;
		timesAArray[i] = 0;
		timesBArray[i] = 0;
	}
	for (int i = 0; i < 9; i++)
	{
		RtimesAArray[i] = 0;
		RtimesBArray[i] = 0;
		RtimesCArray[i] = 0;
	}
}


void StepperMode::stopMoving()
{
	//portENTER_CRITICAL(&mux);
	ledcWrite(pwmCh1, 0);
	ledcWrite(pwmCh2, 0);
	//portEXIT_CRITICAL(&mux);
}

void StepperMode::UpdateInputs()
{
	NOW = IO->GetNowTime();
	SPEED = IO->GetRpm(timesCArray, 10);
	SIDE = IO->GetSide();
	RAWPWM = IO->GetMainPwm(false);
	MAINENABLED = IO->GetMainEnabled();

	CURRENT = IO->GetAmps(1000);
	if (CURRENT < 0)
		CURRENT = 0;
	VOLTAGE = IO->GetVolts(1000);
}

void StepperMode::UpdateOutputs()
{
	PWM = getPwm();
}

int StepperMode::getPwm()
{
		return RAWPWM;
}

void StepperMode::PrintSomeValues() {
	//Serial.print("Pwm: ");
	//Serial.print(MAINPWM);
	//Serial.print("  Dzielnik: ");
	//Serial.print(VOLTAGE);
	//Serial.print("  Prekognicja? ");
	//Serial.print(TIMERSHASPOWER);
	//Serial.print("  kwz: ");
	//Serial.print(kwz);
	Serial.print("  SPEED: ");
	Serial.print(SPEED);
	Serial.print("  MAINENABLED: ");
	Serial.print(MAINENABLED);
	Serial.print("  SIDE: ");
	Serial.print(SIDE);
	Serial.print("  NOW: ");
	Serial.print(NOW);
	Serial.print("  MAINPWM: ");
	Serial.print(RAWPWM);
	Serial.print("  VOLTAGE: ");
	Serial.print(VOLTAGE);
	Serial.print("  CURRENT: ");
	Serial.print(CURRENT);
	//Serial.println();
	Serial.println();
	//Serial.print("Array C:  ");
	/*for (int i = 0; i<10; i++) {
	Serial.print(timesA[i]);
	Serial.print("  ");
	}
	Serial.println();
	for (int i = 0; i<10; i++) {
	Serial.print(timesB[i]);
	Serial.print("  ");
	}*/
	Serial.println();
	//Serial.print("Array C Copy:  ");
	//for (int i = 0; i<10; i++) {
	//Serial.print(timesCcopy[i]);
	//Serial.print("  ");
	//}
}


void IRAM_ATTR StepperMode::makeFakeIRQ()
{
	NpoleWasLast = !NpoleWasLast;
	SpoleWasLast = !SpoleWasLast;
	char side = SIDE;
	if (side == 'R')
		side = 'L';
	else
		side = 'R';
	Mode1PowerIRAM(NpoleWasLast, SpoleWasLast, 255, side);
}