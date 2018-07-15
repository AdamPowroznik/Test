/*
Name:    CLBLDC_ESP32.ino
Created: 6/29/2018 10:12:22 PM
Author:  Adam
Version: 2.0
*/

#include "Conv.h"
#include "ACS712.h"
#include "Voltometer.h"
#include "InputOutput.h"
#include "MovementMode.h"
#include "StaticMomentum.h"
#include "Printer.h"
#include "StaticMomentumPrinter.h"

Conv s(115200); //here I begin Serial port

#include "LiquidCrystal_I2C.h""
#include <Wire.h>



// DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS 

#define button1 5
#define button2 17
#define button3 16
#define button4 4

#define dioda 19
#define diodaB 18
#define enablePin 15
#define potPin 27
#define sideSwitchPin 13
#define voltagePin 12

//pwm definitions
//#define pwmResolution 8
//#define pwmChannel0 0
//#define pwmChannel1 1
//#define motorAPin 33
//#define motorBPin 32
//int freq = 50000;
//
////irq definitions
//#define hall1InterruptPin 25
//#define hall2InterruptPin 26
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

//ACS712 currentMeter(currentMeterPin, 66, 3300, 12);


// GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS GLOBALS 
//
//volatile int changesCounter = 0;
//volatile bool NpoleWasLast = true;
//volatile bool SpoleWasLast;

//
//int NEXTA;
//int NEXTB;
//
//
////volatile bool TIMERSHASPOWER = false;
//volatile bool TIMERSON = false;
////volatile bool FIRSTRUN = true;
//volatile bool TIMERSWORKING = false;
//
//
//volatile int timer2interruptCounter;
//volatile int timer1interruptCounter;
//
//volatile double kwz = 0, kwz2;

//Times
//volatile int TimeA, timeA2, TimeB, timeB2, timeC, TimeC;
//volatile int timesC[10], timesB[10], timesA[10]; // tablice czasow
//volatile int RtimesC[9], RtimesB[9], RtimesA[9]; //roznice do algorytmu
//volatile int timesCcopy[10];
//volatile long ONEROTATETIME;
//volatile double ileStopni;
//volatile int PREKOCOUNTER = 5;

hw_timer_t * timer1 = NULL; // timer
hw_timer_t * timer2 = NULL; // timer2

ACS712 meter1(14, 66, 3300, 12);
Voltometer meter2(voltagePin);
InputOutput InputOutputObj(enablePin, potPin, sideSwitchPin, meter1, meter2);
InputOutput *IO = &InputOutputObj;

StaticMomentum Mode1(InputOutputObj, 33, 32, 25, 26, 0, 1, 50000, 8);
MovementMode *worker = &Mode1;

LiquidCrystal_I2C lcd(0x27, 2, 16);
StaticMomentumPrinter staticMomentumPrinter(lcd, Mode1, "FM12V3A6M");
Printer *printer = &staticMomentumPrinter;

// IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ IRQ 
void IRAM_ATTR hallotronNnow() {
	portENTER_CRITICAL_ISR(&mux);
	worker->hallNIRQ();
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR hallotronSnow() {
	portENTER_CRITICAL_ISR(&mux);
	worker->hallSIRQ();
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR onTimer1() {
	portENTER_CRITICAL_ISR(&mux);
	//timer1interruptCounter++;
	//changePolarity(1, !LastWrittenPhaseA, !LastWrittenPhaseB);
	//if (TIMERSON) {
	//	timerWrite(timer1, 0);
	//	if (LastWrittenPhaseA)
	//	{
	//		timerAlarmWrite(timer1, NEXTA, false);
	//	}
	//	else
	//	{
	//		timerAlarmWrite(timer1, NEXTB, false);
	//	}
	//	timerAlarmEnable(timer1);
	//	//PREKOCOUNTER--;
	//}
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR onTimer2() {
	portENTER_CRITICAL_ISR(&mux);

	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR button1IRQ() {
	portENTER_CRITICAL_ISR(&mux);
	//static unsigned long last_interrupt_time = 0;
	//unsigned long interrupt_time = millis();
	//// If interrupts come faster than 200ms, assume it's a bounce and ignore
	//if (interrupt_time - last_interrupt_time > 200)
	//{
	//	if (changesCounter > 100)
	//	{
	//		TIMERSON = true;
	//		digitalWrite(diodaB, 1);
	//	}
	//	last_interrupt_time = interrupt_time;
	//}
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR button2IRQ() {
	portENTER_CRITICAL_ISR(&mux);
	//static unsigned long last_interrupt_time = 0;
	//unsigned long interrupt_time = millis();
	//// If interrupts come faster than 200ms, assume it's a bounce and ignore
	//if (interrupt_time - last_interrupt_time > 200)
	//{
	//	TIMERSON = false;
	//	TIMERSWORKING = false;
	//	digitalWrite(diodaB, 0);
	//	last_interrupt_time = interrupt_time;
	//}
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR button3IRQ() {
	portENTER_CRITICAL_ISR(&mux);
	//static unsigned long last_interrupt_time = 0;
	//unsigned long interrupt_time = millis();
	//// If interrupts come faster than 200ms, assume it's a bounce and ignore
	//if (interrupt_time - last_interrupt_time > 200)
	//{
	//	//IRQ code here
	//	ileStopni += 0.1;
	//	last_interrupt_time = interrupt_time;
	//}
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR button4IRQ() {
	portENTER_CRITICAL_ISR(&mux);
	//static unsigned long last_interrupt_time = 0;
	//unsigned long interrupt_time = millis();
	//// If interrupts come faster than 200ms, assume it's a bounce and ignore
	//if (interrupt_time - last_interrupt_time > 200)
	//{
	//	//IRQ code here
	//	ileStopni -= 0.1;
	//	//kwz -= 100;
	//	last_interrupt_time = interrupt_time;
	//}
	portEXIT_CRITICAL_ISR(&mux);
}

//ACS712 miernik(14, 66, 3300, 12);


// SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP 
void setup() {
	//Serial.begin(115200);
	worker->pwmSetup();
	pinMode(button1, INPUT_PULLUP);
	pinMode(button2, INPUT_PULLUP);
	pinMode(button3, INPUT_PULLUP);
	pinMode(button4, INPUT_PULLUP);
	pinMode(dioda, OUTPUT);
	pinMode(diodaB, OUTPUT);
	//timer Setup
	timer1 = timerBegin(0, 80, true);
	timerAttachInterrupt(timer1, &onTimer1, true);

	/*timer2 = timerBegin(3, 80, true);
	timerAttachInterrupt(timer2, &onTimer2, true);
	//timerWrite(timer2, 0);
	timerAlarmWrite(timer2, 1, true);
	timerAlarmEnable(timer2);
	*/

	//IRQ Setup
	/*pinMode(hall1InterruptPin, INPUT_PULLUP);
	pinMode(hall2InterruptPin, INPUT_PULLUP);*/
	attachInterrupt(digitalPinToInterrupt(worker->hallNPin), hallotronNnow, RISING);
	attachInterrupt(digitalPinToInterrupt(worker->hallSPin), hallotronSnow, RISING);
	attachInterrupt(digitalPinToInterrupt(button1), button1IRQ, RISING);
	attachInterrupt(digitalPinToInterrupt(button2), button2IRQ, RISING);
	attachInterrupt(digitalPinToInterrupt(button3), button3IRQ, RISING);
	attachInterrupt(digitalPinToInterrupt(button4), button4IRQ, RISING);

	//pwm Setup
	/*pinMode(motorAPin, OUTPUT);
	pinMode(motorBPin, OUTPUT);*/
	/*ledcSetup(pwmChannel0, freq, pwmResolution);
	ledcAttachPin(motorAPin, pwmChannel0);
	ledcSetup(pwmChannel1, freq, pwmResolution);
	ledcAttachPin(motorBPin, pwmChannel1);*/

	/*lcd.begin();
	lcd.clear();*/

}

//InputOutput *IO;


//int printInterval = 2000, lastPrint;
// MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP 

void loop() {
	worker->Work();
	printer->Print();
}

