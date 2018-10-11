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
}

BrakerMode::BrakerMode(InputOutput &IO, int motorNPin, int motorSPin, int hallNPin, int hallSPin, int pwmCh1, int pwmCh2, int pwmFreq, int pwmRes)
	:MovementMode(motorNPin, motorSPin, hallNPin, hallSPin, pwmCh1, pwmCh2, pwmFreq, pwmRes)
{
	this->IO = &IO;
	UpdateInputs();
	Serial.println("Braker Mode configurated sucessfully.");
}


BrakerMode::~BrakerMode()
{
}

void BrakerMode::hallNIRQ()
{
	MovementMode::hallNIRQ();
	if (SpoleWasLast)
		timeSinceLastChange = millis();
}

void BrakerMode::hallSIRQ()
{
	MovementMode::hallSIRQ();
	if(NpoleWasLast)
		timeSinceLastChange = millis();
}

void BrakerMode::Work()
{
	UpdateInputs();


	if (MAINENABLED)
	{
		UpdateOutputs();

		digitalWrite(19, 1);
		if (RAWPWM >= minPwm)
		{
			goLeftIRAM(true, false, PWM);
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

	if (timeSinceLastChange + 1500 < NOW) {
		TOLONGWITHOUTCHANGESTOP();
	}

	if (lastPrint + printInterval < NOW) {
		lastPrint = millis();
		PrintSomeValues();
		//printToLcd();
	}
}

MovementTypes BrakerMode::GetType()
{
	return Braker;
}

void IRAM_ATTR BrakerMode::goLeftIRAM(bool phase1, bool phase2, int pwm)
{
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

void BrakerMode::DONTMOVEINANYCASE()
{
	TOLOWPWMSTOP();
	digitalWrite(19, 0);
}

void BrakerMode::TOLOWPWMSTOP()
{
	stopMoving();
	changesCounter = 0;
	ResetArrays();
	PWM = 0;
}

void BrakerMode::TOLONGWITHOUTCHANGESTOP()
{
	ResetArrays();
	changesCounter = 0;
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
	RAWPWM = IO->GetMainPwm(false);
	RAWPWM += bonusPwmByValue;
	if (RAWPWM > 255) {
		RAWPWM = 255;
		bonusPwmByPercent = 0;
	}
	else if (RAWPWM < minPwm) {
		RAWPWM = minPwm;
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
		return RAWPWM;
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

void BrakerMode::reset()
{
	PWM = 0;
	lastPwm = 0;
	CURRENT = 0;
	VOLTAGE = 0;
	MAINENABLED = false;
	UpdateInputs();
}
