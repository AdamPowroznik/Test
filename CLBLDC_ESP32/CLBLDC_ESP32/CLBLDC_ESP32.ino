/*
Name:    CLBLDC_ESP32.ino
Created: 6/29/2018 10:12:22 PM
Author:  Adam
Version: 1.4
*/


#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 2, 16);

// DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS 

#define button1 5
#define button2 17
#define button3 16
#define button4 4

#define dioda 19
#define diodaB 18
#define enablePin 15
#define potPin 27
#define sidePin 13
#define voltagePin 12

//pwm definitions
#define pwmResolution 8
#define pwmChannel0 0
#define pwmChannel1 1
#define motorAPin 33
#define motorBPin 32
int freq = 50000;

//irq definitions
#define hall1InterruptPin 25
#define hall2InterruptPin 26
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;



// GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS 

volatile int changesCounter = 0;
volatile bool NpoleWasLast = true;
volatile bool SpoleWasLast;
volatile int lastChangeAgo;

bool MAINENABLED;
int MAINPWM;
double SPEED;
double CURRENT;
double VOLTAGE;
int NOW;
char SIDE;
int NEXTA;
int NEXTB;

int MODE = 1;
/*
1: basic
2: still speed
3: basic + prekognition?
4: stepper?
5: brakes?
*/

bool TIMERSHASPOWER = false;
bool TIMERSON = false;
bool FIRSTRUN = true;
bool TIMERSWORKING = false;

volatile int timeSinceLastChange = 0;
volatile int lastPolyChange = 0;
int fakeIRQcounter = 0;
int lastFakeTime = 0;

volatile int timer2interruptCounter;
volatile int timer1interruptCounter;

volatile bool LastWrittenPhaseA;
volatile bool LastWrittenPhaseB;
volatile int kwz = 0, kwz2;

//Times
volatile int TimeA, timeA2, TimeB, timeB2, timeC, TimeC;
volatile int timesC[10], timesB[10], timesA[10]; // tablice czasow
volatile int RtimesC[9], RtimesB[9], RtimesA[9]; //roznice do algorytmu
volatile int timesCcopy[10];
volatile int oneRotate;

hw_timer_t * timer1 = NULL; // timer
hw_timer_t * timer2 = NULL; // timer2

//IN RAM FUNCTIONS IN RAM FUNCTIONS IN RAM FUNCTIONS IN RAM FUNCTIONS IN RAM FUNCTIONS IN RAM FUNCTIONS IN RAM FUNCTIONS IN RAM FUNCTIONS 
// DONT USE IRAM FUNCTIONS IN NORMAL CODE BCS IRQ WILL TRY TO INTERRUPT AND THEN ALL WILL CRASH


void IRAM_ATTR updateArrayC() {
	for (int i = 0; i <= 8; i++) {
		timesC[i] = timesC[i + 1];
	}
	timesC[9] = TimeC;
}

void IRAM_ATTR getTimeC() {
	TimeC = micros() - timeC;
	timeC = micros();
	updateArrayC();
}

void IRAM_ATTR updateArrayB(int lastB) {
	for (int i = 0; i <= 8; i++) {
		timesB[i] = timesB[i + 1];
	}
	timesB[9] = lastB;
}

void IRAM_ATTR updateArrayA(int lastA) {
	for (int i = 0; i <= 8; i++) {
		timesA[i] = timesA[i + 1];
	}
	timesA[9] = lastA;
}

void IRAM_ATTR goLeftIRAM(bool phase1, bool phase2, int pwm) {
	lastPolyChange = millis();
	if (phase1) {
		ledcWrite(pwmChannel0, 0);
		ledcWrite(pwmChannel1, pwm);
	}
	else if (phase2) {
		ledcWrite(pwmChannel1, 0);
		ledcWrite(pwmChannel0, pwm);
	}
}

void IRAM_ATTR goRightIRAM(bool phase1, bool phase2, int pwm) {
	lastPolyChange = millis();
	if (phase1) {
		ledcWrite(pwmChannel1, 0);
		ledcWrite(pwmChannel0, pwm);
	}
	else if (phase2) {
		ledcWrite(pwmChannel0, 0);
		ledcWrite(pwmChannel1, pwm);
	}
}

void IRAM_ATTR Mode1PowerIRAM(bool phase1, bool phase2, int pwm, char side) {
	LastWrittenPhaseA = phase1;
	LastWrittenPhaseB = phase2;
	if (side == 'L') {
		goLeftIRAM(phase1, phase2, pwm);
	}
	else if (side == 'R') {
		goRightIRAM(phase1, phase2, pwm);
	}
}

//int IRAM_ATTR getPwmIRAM(int pin) {
//	return map(analogRead(pin), 0, 4095, 75, 255);
//}

//char IRAM_ATTR getSide(int pin) {
//	if (digitalRead(pin) == LOW)
//		return 'L';
//	else
//		return 'R';
//}

//float IRAM_ATTR getRPMIRAM(uint8_t numberOfTimesToAV, uint8_t magneses) {
//	int avTimeC;
//	int rpm;
//	for (uint8_t i = 0; i < numberOfTimesToAV; i++)
//	{
//		uint8_t j = 9 - i;
//		avTimeC += timesC[j];
//	}
//	avTimeC /= numberOfTimesToAV;
//	oneRotate = avTimeC * magneses / 2;
//
//	rpm = 60000 / oneRotate;
//	return rpm;
//}

long IRAM_ATTR Rav(volatile int array[], int numberOfElements) {
	long Rav;
	for (int i = 0; i < numberOfElements; i++)
	{
		Rav += array[i];
	}
	return (Rav / numberOfElements);
}

void IRAM_ATTR calcRarrayIRAM(char baseArrayName, int numberOfElementsInBaseArray) {
	switch (baseArrayName)
	{
	case 'A':
		for (int i = 0; i < (numberOfElementsInBaseArray - 1); i++)
		{
			RtimesA[i] = timesA[i + 1] - timesA[i];
		}
		break;
	case 'B':
		for (int i = 0; i < (numberOfElementsInBaseArray - 1); i++)
		{
			RtimesB[i] = timesB[i + 1] - timesB[i];
		}
		break;
	case 'C':
		for (int i = 0; i < (numberOfElementsInBaseArray - 1); i++)
		{
			RtimesC[i] = timesC[i + 1] - timesC[i];
		}
		break;
	default:
		break;
	}
}


float IRAM_ATTR NextChangeWillBeOn(int timer) {
	if (timer == 0) {
		calcRarrayIRAM('B', 10);
		return (timesB[9] + Rav(RtimesB, 9));
	}
	else if (timer == 1) {
		calcRarrayIRAM('A', 10);
		return (timesA[9] + Rav(RtimesA, 9));
	}
}

int IRAM_ATTR NextB() {
	calcRarrayIRAM('B', 10);
	return timesB[9] + Rav(RtimesB, 9) - kwz;
}

int IRAM_ATTR NextA() {
	calcRarrayIRAM('A', 10);
	return timesA[9] + Rav(RtimesA, 9) + kwz;
}

//void IRAM_ATTR timerSetups(int timer) {
//  if (timer == 0) {
//    timerWrite(timer1, 0);
//    long czas = NextChangeWillBeOn(timer);
//    //czas += kwz2 * czas;
//    timerAlarmWrite(timer1, czas, false);
//    timerAlarmEnable(timer1);
//  }
//  else if (timer == 1) {
//    timerWrite(timer2, 0);
//    long czas = NextChangeWillBeOn(timer);
//    //czas += kwz2 * czas;
//    timerAlarmWrite(timer2, czas, false);
//    timerAlarmEnable(timer2);
//  }
//}

void IRAM_ATTR changePolarity(char mode, bool phase1, bool phase2) {
	//Mode1PowerIRAM(phase1, phase2, getPwmIRAM(potPin), getSide(sidePin));
	Mode1PowerIRAM(phase1, phase2, MAINPWM, SIDE);
}

bool IRAM_ATTR counterFull(int ovf) {
	if (changesCounter % (ovf + 1) == ovf)
		return true;
	else
		return false;
}


void IRAM_ATTR makeFakeIRQ() {
	// portENTER_CRITICAL(&mux);
	NpoleWasLast = !NpoleWasLast;
	SpoleWasLast = !SpoleWasLast;
	char side = SIDE;
	if (side == 'R')
		side = 'L';
	else
		side = 'R';
	Mode1PowerIRAM(NpoleWasLast, SpoleWasLast, 255, side); //krzak?: true-false? noIRAM?
														   // portEXIT_CRITICAL(&mux);

}

void IRAM_ATTR makeFakeIRQ2() {
	//portENTER_CRITICAL(&mux);
	NpoleWasLast = !NpoleWasLast;
	SpoleWasLast = !SpoleWasLast;
	Mode1PowerIRAM(NpoleWasLast, SpoleWasLast, 255,SIDE); //krzak?: true-false? noIRAM?
																	   // portEXIT_CRITICAL(&mux);

}

// IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ 
void IRAM_ATTR hallotronNnow() {
	portENTER_CRITICAL_ISR(&mux);
	if (SpoleWasLast) {
		changesCounter++;
		timeA2 = micros();
		TimeB = micros() - timeB2;
		updateArrayB(TimeB);
		getTimeC();
	}
	NpoleWasLast = true;
	SpoleWasLast = false;
	if (MAINENABLED)
	{
		if (!TIMERSWORKING && TIMERSON) {
			changePolarity(1, NpoleWasLast, SpoleWasLast);
			TIMERSWORKING = true;
			timerWrite(timer1, 0);
			if (LastWrittenPhaseA)
			timerAlarmWrite(timer1, NEXTA, false);
			else
			timerAlarmWrite(timer1, NEXTB, false);
			timerAlarmEnable(timer1);
		}
		else if(!TIMERSHASPOWER)
			changePolarity(1, NpoleWasLast, SpoleWasLast);
	}
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR hallotronSnow() {
	portENTER_CRITICAL_ISR(&mux);
	if (NpoleWasLast) {
		changesCounter++;
		timeB2 = micros();
		TimeA = micros() - timeA2;
		updateArrayA(TimeA);
	}
	SpoleWasLast = true;
	NpoleWasLast = false;
	if (MAINENABLED)
	{
		if (!TIMERSWORKING && TIMERSON) {
			changePolarity(1, NpoleWasLast, SpoleWasLast);
			TIMERSWORKING = true;
			timerWrite(timer1, 0);
			if (LastWrittenPhaseA)
				timerAlarmWrite(timer1, NEXTA, false);
			else
				timerAlarmWrite(timer1, NEXTB, false);
			timerAlarmEnable(timer1);
		}
		else if(!TIMERSHASPOWER)
			changePolarity(1, NpoleWasLast, SpoleWasLast);		
	}

	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR onTimer1() {
	portENTER_CRITICAL_ISR(&mux);
	timer1interruptCounter++;
	if (TIMERSHASPOWER) {
		//here ill change poly
		changePolarity(1, LastWrittenPhaseB, LastWrittenPhaseA);
	}
	if (TIMERSON) {
		timerWrite(timer1, 0);
		if (LastWrittenPhaseA)
		{
			timerAlarmWrite(timer1, NEXTA, false);  //bardzo mozliwe ze odwrotnie A i B
		}
		else
		{
			timerAlarmWrite(timer1, NEXTB, false);
		}
		timerAlarmEnable(timer1);
	}
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR onTimer2() {
	portENTER_CRITICAL_ISR(&mux);
	
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR button1IRQ() {
	portENTER_CRITICAL_ISR(&mux);
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	// If interrupts come faster than 200ms, assume it's a bounce and ignore
	if (interrupt_time - last_interrupt_time > 200)
	{
		TIMERSHASPOWER = true;
		digitalWrite(diodaB, 1);
		last_interrupt_time = interrupt_time;
	}
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR button2IRQ() {
	portENTER_CRITICAL_ISR(&mux);
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	// If interrupts come faster than 200ms, assume it's a bounce and ignore
	if (interrupt_time - last_interrupt_time > 200)
	{
		TIMERSHASPOWER = false;

		digitalWrite(diodaB, 0);
		last_interrupt_time = interrupt_time;
	}
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR button3IRQ() {
	portENTER_CRITICAL_ISR(&mux);
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	// If interrupts come faster than 200ms, assume it's a bounce and ignore
	if (interrupt_time - last_interrupt_time > 200)
	{
		//IRQ code here
		//kwz++;
		last_interrupt_time = interrupt_time;
	}
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR button4IRQ() {
	portENTER_CRITICAL_ISR(&mux);
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	// If interrupts come faster than 200ms, assume it's a bounce and ignore
	if (interrupt_time - last_interrupt_time > 200)
	{
		//IRQ code here
		//kwz--;
		last_interrupt_time = interrupt_time;
	}
	portEXIT_CRITICAL_ISR(&mux);
}

//CLASSES CLASSES CLASSES CLASSES CLASSES CLASSES CLASSES CLASSES CLASSES CLASSES CLASSES CLASSES CLASSES CLASSES CLASSES CLASSES 
class ACS712 {
private:
	int analogPin;
	int mvPerAmp;
	int baseVoltage;
	int ACSoffset;
	double zeroAmps;
	int RawValue = 0;
	int resolution;
	double Voltage;
	double Amps;

public:
	ACS712(int analogPin, int mvPerAmp, int baseVoltage, int resolution) {
		this->analogPin = analogPin;
		this->mvPerAmp = mvPerAmp;
		this->baseVoltage = baseVoltage;
		this->resolution = resolution;
	}

	double getAmps(int samples) {
		for (int i = 0; i<samples; i++)
		{
			RawValue = analogRead(analogPin);
			Voltage = (RawValue / pow(2, resolution)) * baseVoltage;
			Amps += (((Voltage - ACSoffset) / mvPerAmp) - zeroAmps);
		}
		Amps /= samples;
		return Amps;
	}

	void Setupb(int samplesToOffset, int samplesToZeroAmp) {
		for (int i = 0; i<samplesToOffset; i++) {
			ACSoffset += analogRead(analogPin);
		}
		ACSoffset /= samplesToOffset;

		for (int i = 0; i<samplesToZeroAmp; i++)
		{
			RawValue = analogRead(analogPin);
			double zeroVoltage = (RawValue / pow(2, resolution)) * baseVoltage;
			zeroAmps += ((zeroVoltage - ACSoffset) / mvPerAmp);
		}
		zeroAmps /= samplesToZeroAmp;
	}
};

ACS712 miernik(14, 66, 3300, 12);
// SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP 
void setup() {
	Serial.begin(115200);
	pinMode(potPin, INPUT);
	pinMode(sidePin, INPUT_PULLUP);
	pinMode(button1, INPUT_PULLUP);
	pinMode(button2, INPUT_PULLUP);
	pinMode(button3, INPUT_PULLUP);
	pinMode(button4, INPUT_PULLUP);
	pinMode(dioda, OUTPUT);
	pinMode(enablePin, INPUT_PULLUP);
	pinMode(voltagePin, INPUT);
	pinMode(diodaB, OUTPUT);
	//timer Setup
	timer1 = timerBegin(2, 80, true);
	timerAttachInterrupt(timer1, &onTimer1, true);
	
	/*timer2 = timerBegin(3, 80, true);
	timerAttachInterrupt(timer2, &onTimer2, true);
	//timerWrite(timer2, 0);
	timerAlarmWrite(timer2, 1, true);
	timerAlarmEnable(timer2);
	*/

	//IRQ Setup
	pinMode(hall1InterruptPin, INPUT_PULLUP);
	pinMode(hall2InterruptPin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(hall1InterruptPin), hallotronNnow, RISING);
	attachInterrupt(digitalPinToInterrupt(hall2InterruptPin), hallotronSnow, RISING);
	attachInterrupt(digitalPinToInterrupt(button1), button1IRQ, RISING);
	attachInterrupt(digitalPinToInterrupt(button2), button2IRQ, RISING);
	attachInterrupt(digitalPinToInterrupt(button3), button3IRQ, RISING);
	attachInterrupt(digitalPinToInterrupt(button4), button4IRQ, RISING);

	//pwm Setup
	pinMode(motorAPin, OUTPUT);
	pinMode(motorBPin, OUTPUT);
	ledcSetup(pwmChannel0, freq, pwmResolution);
	ledcAttachPin(motorAPin, pwmChannel0);
	ledcSetup(pwmChannel1, freq, pwmResolution);
	ledcAttachPin(motorBPin, pwmChannel1);

	lcd.begin();
	lcd.clear();

	miernik.Setupb(100, 1000);
}

int printInterval = 2000, lastPrint;
// MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP 

void loop() {
	UpdateInputs();
	UpdateOutputs();


	if (MAINENABLED)
	{
		digitalWrite(dioda, 1);
		timeSinceLastChange = NOW - lastPolyChange;
		if (MAINPWM >= 100)
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
			if (!TIMERSON && changesCounter > 100)
			{
				TIMERSON = true;
			}
			if (NOW - lastFakeTime > 1000) {
				fakeIRQcounter = 0;
			}
		}
		else if (MAINPWM <= 80)
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
		printToLcd();
	}
}
// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC

void DONTMOVEINANYCASE() {
	TOLOWPWMSTOP();
	digitalWrite(dioda, 0);
}

void TOLOWPWMSTOP() {
	FIRSTRUN = true;
	stopMoving();
	changesCounter = 0;
	ResetArrays();
	TIMERSHASPOWER = false;
	TIMERSON = false;
	TIMERSWORKING = false;
	fakeIRQcounter = 0;
	lastFakeTime = 0;
	timeSinceLastChange = -1;
	lastPolyChange = 0;
}

void TOLONGWITHOUTCHANGESTOP() {
	ResetArrays();
	changesCounter = 0;
	TIMERSHASPOWER = false;
	TIMERSON = false;
	TIMERSWORKING = false;
	timeSinceLastChange = -1;//NOT SURE ABOUT THOSE TWO
	lastPolyChange = 0;//NOT SURE ABOUT THOSE TWO
}

void RETRYMOVING() {
	if (fakeIRQcounter < 3)
		makeFakeIRQ();
	else if (fakeIRQcounter >= 3)
		makeFakeIRQ2();

	if (NOW - lastFakeTime > 100)
		fakeIRQcounter++;
	lastFakeTime = millis();
}

void PrintSomeValues() {
	//Serial.print("Pwm: ");
	//Serial.print(MAINPWM);
	//Serial.print("  Dzielnik: ");
	//Serial.print(VOLTAGE);
	Serial.print("  Prekognicja? ");
	Serial.print(TIMERSHASPOWER);
	//Serial.print("  kwz: ");
	//Serial.print(kwz);
	Serial.print("  Speed: ");
	Serial.print(SPEED);
	Serial.print("  T1: ");
	Serial.print(timer1interruptCounter);
	Serial.print("  NextA: ");
	Serial.print(NextA());
	Serial.print("  NextB: ");
	Serial.print(NextB());
	//Serial.print("  NextChangeWillBeOn: ");
	//Serial.println();
	Serial.println();
	//Serial.print("Array C:  ");
for (int i = 0; i<10; i++) {
Serial.print(timesA[i]);
Serial.print("  ");
}
Serial.println();
for (int i = 0; i<10; i++) {
	Serial.print(timesB[i]);
	Serial.print("  ");
}
Serial.println();
//Serial.print("Array C Copy:  ");
//for (int i = 0; i<10; i++) {
//Serial.print(timesCcopy[i]);
//Serial.print("  ");
//}
}

void printToLcd() {
	lcd.clear();
	lcd.setCursor(0, 1);
	lcd.print("RPM:");
	lcd.print(SPEED, 0);
	lcd.setCursor(0, 2);
	lcd.print("Curr:");
	lcd.print(CURRENT, 2);
	lcd.print("A");
	lcd.setCursor(0, 3);
	lcd.print("Vol:");
	lcd.print(VOLTAGE, 2);
	lcd.print("V");
}

//bool IsArrayAcomplete(int numberOfElements) {
//	char counter = 0;
//	for (int i = 0; i < numberOfElements; i++)
//	{
//		if (timesA[i] != 0)
//			counter++;
//	}
//	if (counter == numberOfElements)
//		return true;
//	else
//		return false;
//}
//
//bool IsArrayBcomplete(int numberOfElements) {
//	char counter = 0;
//	for (int i = 0; i < numberOfElements; i++)
//	{
//		if (timesB[i] != 0)
//			counter++;
//	}
//	if (counter == numberOfElements)
//		return true;
//	else
//		return false;
//}
//
//bool IsArrayCcomplete(int numberOfElements) {
//	char counter = 0;
//	for (int i = 0; i < numberOfElements; i++)
//	{
//		if (timesC[i] != 0)
//			counter++;
//	}
//	if (counter == numberOfElements)
//		return true;
//	else
//		return false;
//}


double GetRPM(int numberOfTimesToAV, uint8_t magneses)
{
	if (changesCounter >= 20) {
		unsigned long avTimeC;
		copyTimesC();
		for (int i = 0; i < numberOfTimesToAV; i++)
		{
			avTimeC += timesCcopy[i];
		}
		avTimeC /= numberOfTimesToAV;
		unsigned long oneRotate = avTimeC * 6 / 2;

		double rpm = 60000000 / oneRotate;
		return rpm;
	}
	else return 0;
	
}

void ResetArrays() {
	/*for (int i = 0; i <10; i++) {
		timesC[i] = 0;
		timesA[i] = 0;
		timesB[i] = 0;
	}
	for (int i = 0; i < 9; i++)
	{
		RtimesA[i] = 0;
		RtimesB[i] = 0;
		RtimesC[i] = 0;
	}
	*/
	volatile int timesC[10], timesB[10], timesA[10]; // tablice czasow
	volatile int RtimesC[9], RtimesB[9], RtimesA[9];
}

// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE 

//
//int Rmax(volatile int array[], int numberOfElements) {
//	int Rmax = array[0];
//	for (int i = 1; i < numberOfElements; i++)
//	{
//		Rmax = max(Rmax, array[i]);
//	}
//	return Rmax;
//}
//
//int Rmin(volatile int array[], int numberOfElements) {
//	int Rmin = array[0];
//	for (int i = 1; i < numberOfElements; i++)
//	{
//		Rmin = min(Rmin, array[i]);
//	}
//	return Rmin;
//}
//
//double rav(volatile int TimesArray[], int numberOElements) {
//	return ((TimesArray[numberOElements - 1] - TimesArray[numberOElements - 2] - TimesArray[1] + TimesArray[0]) / (numberOElements - 2));
//}
//
//int Rcharacter(volatile int array[], int numberOfElements) {
//	if (((Rmin(array, numberOfElements) / Rav(array, numberOfElements)) >= 0.7) || ((Rmax(array, numberOfElements) / Rav(array, numberOfElements)) <= 1.3))
//		return 0; // R const
//	else {
//		char counter;
//		for (int i = 1; i < numberOfElements; i++)
//		{
//			if (array[i - 1] < array[i])
//				counter++;
//		}
//		if (counter == numberOfElements)
//			return 2; //R falling
//		else {
//			counter = 0;
//			for (int i = 1; i < numberOfElements; i++)
//			{
//				if (array[i - 1] < array[i])
//					counter++;
//			}
//			if (counter == numberOfElements)
//				return 3; //R rising}
//			else
//				return -1; //R undefined
//		}
//	}
//}

void beginMoving() {
	portENTER_CRITICAL(&mux);
	int pwm = MAINPWM;
	if (pwm < 100)
		pwm = 100;
	Mode1PowerIRAM(!SpoleWasLast, !NpoleWasLast, pwm, SIDE);   //krzak?: true-false? noIRAM?
	portEXIT_CRITICAL(&mux);
}

void stopMoving() {
	portENTER_CRITICAL(&mux);
	ledcWrite(pwmChannel0, 0);
	ledcWrite(pwmChannel1, 0);
	portEXIT_CRITICAL(&mux);
}

void copyTimesC() {
	portENTER_CRITICAL(&mux);
	for (int i = 0; i <10; i++) {
		timesCcopy[i] = timesC[i];
	}
	portEXIT_CRITICAL(&mux);
}

double setVoltage(int samples) {
	double voltage;
	for (int i = 0; i<samples; i++)
	{
			voltage = voltage + map(analogRead(voltagePin), 0, 4095, 0, 42);
	}
	return voltage / samples;
}

void UpdateInputs() {
	setEnabled();
	setPwm();
	setNow();
	setSide();
}

void UpdateOutputs() {
	CURRENT = miernik.getAmps(1000);
	VOLTAGE = setVoltage(10000);
	SPEED = GetRPM(10, 6);
	NEXTA = NextA();
	NEXTB = NextB();
}

void setSide() {
	if (digitalRead(sidePin) == LOW)
		SIDE = 'L';
	else
		SIDE = 'R';
}

void setEnabled() {
	if (digitalRead(enablePin) == LOW)
		MAINENABLED = false;
	else MAINENABLED = true;
}

void setPwm() {
	MAINPWM = map(analogRead(potPin), 0, 4095, 75, 255);
}

void setNow() {
	NOW = millis();
}