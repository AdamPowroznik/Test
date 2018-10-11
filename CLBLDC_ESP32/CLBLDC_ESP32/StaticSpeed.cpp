#include "StaticSpeed.h"

StaticSpeed::StaticSpeed(InputOutput &IO, int motorNPin, int motorSPin, int hallNPin, int hallSPin, int pwmCh1, int pwmCh2, int pwmFreq, int pwmRes)
	:MovementMode(motorNPin, motorSPin, hallNPin, hallSPin, pwmCh1, pwmCh2, pwmFreq, pwmRes)
{
	this->IO = &IO;
	UpdateInputs();
	Serial.println("Static Speed Mode configurated sucessfully.");
}


StaticSpeed::~StaticSpeed()
{
}

void StaticSpeed::hallNIRQ()
{
	MovementMode::hallNIRQ();
	/*static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	if (interrupt_time - last_interrupt_time > IO->oneRotate) {
		WANTEDPWM = IO->GetWantedPwm(WANTEDSPEED, SPEED, WANTEDPWM);
		last_interrupt_time = interrupt_time;
	}*/
	
	/*oneRotateCounter++;
	if (oneRotateCounter >= 6) {
		WANTEDPWM = IO->GetWantedPwm(WANTEDSPEED, SPEED, WANTEDPWM);
		oneRotateCounter = 0;
	}*/
	if (MAINENABLED) {
		if (changesCounter < 100)
			Mode1PowerIRAM(NpoleWasLast, SpoleWasLast, NORMALPWM, SIDE);
		else {
			Mode1PowerIRAM(NpoleWasLast, SpoleWasLast, WANTEDPWM, SIDE);
		}

	}
		
}

void StaticSpeed::hallSIRQ()
{
	MovementMode::hallSIRQ();
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	if (interrupt_time - last_interrupt_time > 300 && MAINENABLED) {
		WANTEDPWM = IO->GetWantedPwm(WANTEDSPEED, SPEED, WANTEDPWM, softStart, lastPwm);
		last_interrupt_time = interrupt_time;
	}

	/*oneRotateCounter++;
	if (oneRotateCounter >= 6) {
	WANTEDPWM = IO->GetWantedPwm(WANTEDSPEED, SPEED, WANTEDPWM);
	oneRotateCounter = 0;
	}*/
	if (MAINENABLED) {
		if (changesCounter < 200)
			Mode1PowerIRAM(NpoleWasLast, SpoleWasLast, NORMALPWM, SIDE);
		else {
			Mode1PowerIRAM(NpoleWasLast, SpoleWasLast, WANTEDPWM, SIDE);
		}

	}
}

void StaticSpeed::Work()
{
	UpdateInputs();

	if (MAINENABLED)
	{
		digitalWrite(19, 1);
		timeSinceLastChange = NOW - lastPolyChange;
		if (WANTEDSPEED >= 300)
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
		if (timeSinceLastChange > 1500)
		{
			TOLONGWITHOUTCHANGESTOP();
		}
		else if (WANTEDSPEED <= 250)
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

MovementTypes StaticSpeed::GetType()
{
	return Speed;
}

void IRAM_ATTR StaticSpeed::goLeftIRAM(bool phase1, bool phase2, int pwm)
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

void IRAM_ATTR StaticSpeed::goRightIRAM(bool phase1, bool phase2, int pwm)
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

void IRAM_ATTR StaticSpeed::Mode1PowerIRAM(bool phase1, bool phase2, int pwm, char side)
{
	lastPwm = pwm;
	changesCounter++;
	LastWrittenPhaseA = phase1;
	LastWrittenPhaseB = phase2;
	if (side == 'L') {
		goLeftIRAM(phase1, phase2, pwm);
	}
	else if (side == 'R') {
		goRightIRAM(phase1, phase2, pwm);
	}
}

void IRAM_ATTR StaticSpeed::makeFakeIRQ()
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

void IRAM_ATTR StaticSpeed::makeFakeIRQ2()
{
	NpoleWasLast = !NpoleWasLast;
	SpoleWasLast = !SpoleWasLast;
	Mode1PowerIRAM(NpoleWasLast, SpoleWasLast, 255, SIDE);
}

void StaticSpeed::DONTMOVEINANYCASE()
{
	TOLOWPWMSTOP();
	ResetArrays();
	digitalWrite(19, 0);
	IO->mainPwm = minPwm;
}

void StaticSpeed::TOLOWPWMSTOP()
{
	FIRSTRUN = true;
	//stopMoving();
	changesCounter = 0;
	WANTEDSPEED = 0;
	fakeIRQcounter = 0;
	lastFakeTime = 0;
	timeSinceLastChange = -1;
	lastPolyChange = 0;
}

void StaticSpeed::TOLONGWITHOUTCHANGESTOP()
{
	ResetArrays();
	changesCounter = 0;
	timeSinceLastChange = -1;//NOT SURE ABOUT THOSE TWO
	lastPolyChange = 0;//NOT SURE ABOUT THOSE TWO
}

void StaticSpeed::RETRYMOVING()
{
	if (fakeIRQcounter < 3)
		makeFakeIRQ();
	else if (fakeIRQcounter >= 3)
		makeFakeIRQ2();

	if (NOW - lastFakeTime > 100)
		fakeIRQcounter++;
	lastFakeTime = millis();
}

void StaticSpeed::ResetArrays()
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

void StaticSpeed::beginMoving()
{
	//portENTER_CRITICAL(&mux);
	int pwm = NORMALPWM;
	if (pwm < 100)
		pwm = 100;
	Mode1PowerIRAM(!SpoleWasLast, !NpoleWasLast, pwm, SIDE);   //krzak?: true-false? noIRAM?
															   //portEXIT_CRITICAL(&mux);
}

void StaticSpeed::stopMoving()
{
	//portENTER_CRITICAL(&mux);
	ledcWrite(pwmCh1, 0);
	ledcWrite(pwmCh2, 0);
	//portEXIT_CRITICAL(&mux);
}

void StaticSpeed::UpdateInputs()
{
	NOW = IO->GetNowTime();
	SPEED = IO->GetRpm(timesCArray, 10);
	SIDE = IO->GetSide();
	MAINENABLED = IO->GetMainEnabled();
	CURRENT = IO->GetAmps(1000);
	if (CURRENT < 0)
		CURRENT = 0;
	VOLTAGE = IO->GetVolts(1000);
	WANTEDSPEED = IO->GetWantedSpeed();
	NORMALPWM = IO->GetMainPwm(softStart);
	//PWM = IO->GetWantedPwm(WANTEDSPEED, SPEED, PWM);
}



void StaticSpeed::PrintSomeValues() {
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
	Serial.print(NORMALPWM);
	Serial.print("  VOLTAGE: ");
	Serial.print(VOLTAGE);
	Serial.print("  CURRENT: ");
	Serial.print(CURRENT);
	Serial.print("  WANTEDSPEED: ");
	Serial.print(WANTEDSPEED);
	Serial.print("  PWM: ");
	Serial.print(WANTEDPWM);
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

void StaticSpeed::setParameter(bool trueup)
{
	return;
}

void StaticSpeed::reset()
{
	DONTMOVEINANYCASE();
	WANTEDPWM = 0;
	NORMALPWM = 0;
	FIRSTRUN = true;
	CURRENT = 0;
	VOLTAGE = 0;
	MAINENABLED = false;
	UpdateInputs();
}

//void StaticMomentum::printToLcd() {
//	lcd.clear();
//	lcd.setCursor(0, 1);
//	lcd.print("RPM:");
//	lcd.print(SPEED, 0);
//	lcd.setCursor(0, 2);
//	lcd.print("Curr:");
//	lcd.print(CURRENT, 2);
//	lcd.print("A");
//	lcd.setCursor(0, 3);
//	lcd.print("Vol:");
//	lcd.print(VOLTAGE, 2);
//	lcd.print("V");
//}
