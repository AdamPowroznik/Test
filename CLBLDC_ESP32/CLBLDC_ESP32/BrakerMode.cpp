#include "BrakerMode.h"


void BrakerMode::setParameter(bool trueup)
{
	if (trueup) {
		bonusPwmByPercent++;
	}
	else {
		bonusPwmByPercent--;
	}
}

void BrakerMode::calcBonusPwmByPercent()
{
	bonusPwmByValue = (bonusPwmByPercent * 255) / 100;
	/*Serial.print(bonusPwmByPercent);
	Serial.print("		");
	Serial.println(bonusPwmByValue);*/
}

BrakerMode::BrakerMode(InputOutput &IO, int motorNPin, int motorSPin, int hallNPin, int hallSPin, int pwmCh1, int pwmCh2, int pwmFreq, int pwmRes)
	:MovementMode(motorNPin, motorSPin, hallNPin, hallSPin, pwmCh1, pwmCh2, pwmFreq, pwmRes)
{
	this->IO = &IO;
	UpdateInputs();
	Serial.println("Static Momentum Mode configurated sucessfully.");
}


BrakerMode::~BrakerMode()
{
}

void BrakerMode::hallNIRQ()
{
	BrakerMode::hallNIRQ();
	if (MAINENABLED)
		Mode1PowerIRAM(NpoleWasLast, SpoleWasLast, PWM, SIDE);
}

void BrakerMode::hallSIRQ()
{
	MovementMode::hallSIRQ();
	if (MAINENABLED)
		Mode1PowerIRAM(NpoleWasLast, SpoleWasLast, PWM, SIDE);
}

void BrakerMode::Work()
{
	UpdateInputs();


	if (MAINENABLED)
	{
		UpdateOutputs();
		digitalWrite(19, 1);
		timeSinceLastChange = NOW - lastPolyChange;
		if (RAWPWM >= minPwm)
		{
			if (FIRSTRUN)
			{
				beginMoving();
				FIRSTRUN = false;
			}
			else if (timeSinceLastChange > 1000)
			{
				TOLONGWITHOUTCHANGESTOP();
				RETRYMOVING();
			}
			else;
			if (NOW - lastFakeTime > 1000) {
				fakeIRQcounter = 0;
			}
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

MovementTypes BrakerMode::GetType()
{
	return Momentum;
}

void IRAM_ATTR BrakerMode::goLeftIRAM(bool phase1, bool phase2, int pwm)
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

void IRAM_ATTR BrakerMode::goRightIRAM(bool phase1, bool phase2, int pwm)
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

void IRAM_ATTR BrakerMode::Mode1PowerIRAM(bool phase1, bool phase2, int pwm, char side)
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

void IRAM_ATTR BrakerMode::makeFakeIRQ()
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

void IRAM_ATTR BrakerMode::makeFakeIRQ2()
{
	NpoleWasLast = !NpoleWasLast;
	SpoleWasLast = !SpoleWasLast;
	Mode1PowerIRAM(NpoleWasLast, SpoleWasLast, 255, SIDE);
}

void BrakerMode::DONTMOVEINANYCASE()
{
	TOLOWPWMSTOP();
	digitalWrite(19, 0);
}

void BrakerMode::TOLOWPWMSTOP()
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

void BrakerMode::TOLONGWITHOUTCHANGESTOP()
{
	ResetArrays();
	changesCounter = 0;
	timeSinceLastChange = -1;//NOT SURE ABOUT THOSE TWO
	lastPolyChange = 0;//NOT SURE ABOUT THOSE TWO
}

void BrakerMode::RETRYMOVING()
{
	if (fakeIRQcounter < 3)
		makeFakeIRQ();
	else if (fakeIRQcounter >= 3)
		makeFakeIRQ2();

	if (NOW - lastFakeTime > 100)
		fakeIRQcounter++;
	lastFakeTime = millis();
}

void BrakerMode::ResetArrays()
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

void BrakerMode::beginMoving()
{
	//portENTER_CRITICAL(&mux);
	int pwm = RAWPWM;
	if (pwm < 100)
		pwm = 100;
	Mode1PowerIRAM(!SpoleWasLast, !NpoleWasLast, pwm, SIDE);   //krzak?: true-false? noIRAM?
															   //portEXIT_CRITICAL(&mux);
}

void BrakerMode::stopMoving()
{
	//portENTER_CRITICAL(&mux);
	ledcWrite(pwmCh1, 0);
	ledcWrite(pwmCh2, 0);
	//portEXIT_CRITICAL(&mux);
}

void BrakerMode::UpdateInputs()
{
	NOW = IO->GetNowTime();
	SPEED = IO->GetRpm(timesCArray, 10);
	if (this->SIDE != IO->GetSide()) {
		changeSide();
	}
	else
		RAWPWM = IO->GetMainPwm(softStart);
	RAWPWM += bonusPwmByValue;
	if (RAWPWM > 255) {
		RAWPWM = 255;
		bonusPwmByPercent = 0;
	}
	else if (RAWPWM < minPwm - 20) {
		RAWPWM = minPwm - 20;
		bonusPwmByPercent = 0;
	}
	MAINENABLED = IO->GetMainEnabled();

	CURRENT = IO->GetAmps(1000);
	if (CURRENT < 0)
		CURRENT = 0;
	VOLTAGE = IO->GetVolts(1000);
}

void BrakerMode::UpdateOutputs()
{
	calcBonusPwmByPercent();
	PWM = getPwm();
}

int BrakerMode::getPwm()
{
	if (!softStart) {
		return RAWPWM;
	}
	else {
		return SoftStart::GetPwmSoft(lastPwm, RAWPWM);
	}
}

void BrakerMode::changeSide()
{
	if (softStop());
	else {
		SIDE = IO->GetSide();
	}
}

bool BrakerMode::softStop()
{
	if (RAWPWM > minPwm) {
		RAWPWM = SoftStart::GetPwmSoft(this->RAWPWM, 0);
		return true;
	}
	return false;
}



void BrakerMode::PrintSomeValues() {
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
