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
#include "StaticSpeed.h"
#include "Printer.h"
#include "StaticMomentumPrinter.h"
#include "StaticSpeedPrinter.h"
#include "SoftStart.h"



Conv s(115200); //here I begin Serial port

#include "LiquidCrystal_I2C.h"
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


portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;


//hw_timer_t * timer1 = NULL; // timer
//hw_timer_t * timer2 = NULL; // timer2

ACS712 meter1(14, 66, 3300, 12);
Voltometer meter2(voltagePin);
InputOutput InputOutputObj(enablePin, potPin, sideSwitchPin, meter1, meter2);
InputOutput *IO = &InputOutputObj;

StaticMomentum Mode1(InputOutputObj, 33, 32, 25, 26, 0, 1, 50000, 8);
StaticSpeed Mode2(InputOutputObj, 33, 32, 25, 26, 0, 1, 50000, 8);
MovementMode *worker = &Mode1;

LiquidCrystal_I2C lcd(0x27, 2, 16);
StaticMomentumPrinter staticMomentumPrinter(lcd, Mode1, "FM12V3A6M");
StaticSpeedPrinter staticSpeedPrinter(lcd, Mode2, "FM12V3A6M", 1600);
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
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	// If interrupts come faster than 200ms, assume it's a bounce and ignore
	if (interrupt_time - last_interrupt_time > 200)
	{
		if (worker->GetType() == Speed) {
			worker = &Mode1;
			printer = &staticMomentumPrinter;
		}
		else if (worker->GetType() == Momentum) {
			worker = &Mode2;
			printer = &staticSpeedPrinter;
		}
		printer->InitPrint();
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
		worker->setParameter(false);
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
		worker->setParameter(true);
		last_interrupt_time = interrupt_time;
	}
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
	worker->Begin();
	printer->InitPrint();
	pinMode(button1, INPUT_PULLUP);
	pinMode(button2, INPUT_PULLUP);
	pinMode(button3, INPUT_PULLUP);
	pinMode(button4, INPUT_PULLUP);
	pinMode(dioda, OUTPUT);
	pinMode(diodaB, OUTPUT);
	//timer Setup
	//timer1 = timerBegin(0, 80, true);
	//timerAttachInterrupt(timer1, &onTimer1, true);

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

}



// MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP 

void loop() {
	worker->Work();
	printer->Print();
}

