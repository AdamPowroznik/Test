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
#include "BrakerModePrinter.h"
#include "ErrorHandlerPrinter.h"
#include "SoftStart.h"
#include "BrakerMode.h"
#include "StepperMode.h"
#include "ErrorHandler.h"

////TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY 
//#include <DS1302.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
//#define ONE_WIRE_BUS 23
//#define TEMPERATURE_PRECISION 12
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
//int numberOfDevices;
//DeviceAddress tempDeviceAddress;
//int lastPrint;
////!TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY 

Conv s(115200); //here I begin Serial port

#include "LiquidCrystal_I2C.h"
#include <Wire.h>



				// DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS DEFINITIONS 


#define EnabledPin 15
#define PotPin 27
#define SidePin 13
#define CurrentMeterPin 14
#define VoltageMeterPin 12
#define HallNPin 25
#define HallSPin 26
#define MotorNPin 33
#define MotorSPin 32
#define BuzzerPin 2
#define TemperatureMeterPin 23
#define Button1Pin 5
#define Button2Pin 17
#define Button3Pin 16
#define Button4Pin 4
#define GreenDiodePin 19
#define BlueDiodePin 18
#define LcdI2CSDA 21
#define LcdI2CSCL 22


portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;


//hw_timer_t * timer1 = NULL; // timer
//hw_timer_t * timer2 = NULL; // timer2

bool ERROR;

ACS712 meter1(CurrentMeterPin, 66, 3300, 12);
Voltometer meter2(VoltageMeterPin);
InputOutput InputOutputObj(EnabledPin, PotPin, SidePin, meter1, meter2);
InputOutput *IO = &InputOutputObj;

StaticMomentum Mode1(InputOutputObj, 33, 32, 25, 26, 0, 1, 50000, 8);
StaticSpeed Mode2(InputOutputObj, 33, 32, 25, 26, 0, 1, 50000, 8);
BrakerMode Mode3(InputOutputObj, 33, 32, 25, 26, 0, 1, 50000, 8);
ErrorHandler errorHandler(InputOutputObj, 33, 32, 25, 26, 0, 1, 50000, 8);
MovementMode *worker = &Mode1;

LiquidCrystal_I2C lcd(0x27, 2, 16);
StaticMomentumPrinter staticMomentumPrinter(lcd, Mode1, "FM12V3A6M");
BrakerModePrinter brakerModePrinter(lcd, Mode3, "FM12V3A6M");
StaticSpeedPrinter staticSpeedPrinter(lcd, Mode2, "FM12V3A6M", 1600);
ErrorHandlerPrinter errorHandlerPrinter(lcd, errorHandler);
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
	//  timerWrite(timer1, 0);
	//  if (LastWrittenPhaseA)
	//  {
	//    timerAlarmWrite(timer1, NEXTA, false);
	//  }
	//  else
	//  {
	//    timerAlarmWrite(timer1, NEXTB, false);
	//  }
	//  timerAlarmEnable(timer1);
	//  //PREKOCOUNTER--;
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
		if (!IO->GetMainEnabled()) {
			if (worker->GetType() == Momentum) {
				worker = &Mode2;
				printer = &staticSpeedPrinter;
			}
			else if (worker->GetType() == Speed) {
				worker = &Mode3;
				printer = &brakerModePrinter;
			}
			else if (worker->GetType() == Braker) {
				worker = &Mode1;
				printer = &staticMomentumPrinter;
			}
			printer->InitPrint();
		}
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
		if (worker->GetType() == Error && errorHandler.deletingError == true) {
			errorHandlerPrinter.GoodbayPrint();
			ERROR = false;
			worker = &Mode1;
			printer = &staticMomentumPrinter;
			printer->InitPrint();
			digitalWrite(2, LOW);
			worker->reset();
		}

		worker->setParameter(true);
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
		if (worker->GetType() == Error) {
			errorHandler.deletingError = true;
			errorHandlerPrinter.AskPrint();
		}
		last_interrupt_time = interrupt_time;
	}
	portEXIT_CRITICAL_ISR(&mux);
}

//ACS712 miernik(14, 66, 3300, 12);


// SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP SETUP 
void setup() {
	//Serial.begin(115200);
	worker->Begin();
	printer->InitPrint();
	pinMode(Button1Pin, INPUT_PULLUP);
	pinMode(Button2Pin, INPUT_PULLUP);
	pinMode(Button3Pin, INPUT_PULLUP);
	pinMode(Button4Pin, INPUT_PULLUP);
	pinMode(BlueDiodePin, OUTPUT);
	pinMode(GreenDiodePin, OUTPUT);
	//timer Setup
	//timer1 = timerBegin(0, 80, true);
	//timerAttachInterrupt(timer1, &onTimer1, true);

	/*timer2 = timerBegin(3, 80, true);
	timerAttachInterrupt(timer2, &onTimer2, true);
	//timerWrite(timer2, 0);
	timerAlarmWrite(timer2, 1, true);
	timerAlarmEnable(timer2);
	*/
	pinMode(2, OUTPUT);
	//IRQ Setup
	/*pinMode(hall1InterruptPin, INPUT_PULLUP);
	pinMode(hall2InterruptPin, INPUT_PULLUP);*/
	attachInterrupt(digitalPinToInterrupt(worker->hallNPin), hallotronNnow, RISING);
	attachInterrupt(digitalPinToInterrupt(worker->hallSPin), hallotronSnow, RISING);
	attachInterrupt(digitalPinToInterrupt(Button1Pin), button1IRQ, RISING);
	attachInterrupt(digitalPinToInterrupt(Button2Pin), button2IRQ, RISING);
	attachInterrupt(digitalPinToInterrupt(Button3Pin), button3IRQ, RISING);
	attachInterrupt(digitalPinToInterrupt(Button4Pin), button4IRQ, RISING);

	////TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY 
	//Serial.println("Dallas Temperature IC Control Library");
	//sensors.begin();
	//numberOfDevices = sensors.getDeviceCount();
	//Serial.print("Locating devices...");
	//Serial.print("Found ");
	//Serial.print(numberOfDevices, DEC);
	//Serial.println(" devices.");
	//Serial.print("Parasite power is: ");
	//if (sensors.isParasitePowerMode()) Serial.println("ON");
	//else Serial.println("OFF");
	//for (int i = 0; i<numberOfDevices; i++)
	//{
	//	if (sensors.getAddress(tempDeviceAddress, i))
	//	{
	//		Serial.print("Found device ");
	//		Serial.print(i, DEC);
	//		Serial.print(" with address: ");
	//		printAddress(tempDeviceAddress);
	//		Serial.println();
	//		Serial.print("Setting resolution to ");
	//		Serial.println(TEMPERATURE_PRECISION, DEC);
	//		sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
	//		Serial.print("Resolution actually set to: ");
	//		Serial.print(sensors.getResolution(tempDeviceAddress), DEC);
	//		Serial.println();
	//	}
	//	else {
	//		Serial.print("Found ghost device at ");
	//		Serial.print(i, DEC);
	//		Serial.print(" but could not detect address. Check power and cabling");
	//	}
	//}
	//delay(5000);
	////!TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY 


}



// MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP 

void loop() {
	if (errorHandler.Monitor() && !ERROR) {
		ERROR = true;
		worker->reset();
		worker = &errorHandler;
		printer = &errorHandlerPrinter;
		errorHandlerPrinter.InitPrint();
	}
	worker->Work();
	printer->Print();




	////TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY 
	//if (lastPrint + 1000 < IO->GetNowTime()) {
	//	lastPrint = millis();
	//	sensors.requestTemperatures();

	//	float temp = sensors.getTempC(&tempDeviceAddress[0]);
	//	Serial.println(temp);
	//}

	////!TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY 

}
























////TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY TEMPERATURY 
//void printTemperature(DeviceAddress deviceAddress)
//{
//	float tempC = sensors.getTempC(deviceAddress);
//	Serial.print("Temp C: ");
//	Serial.println(tempC);
//	Serial.print("Temp F: ");
//	Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
//}
//
//// function to print a device address
//void printAddress(DeviceAddress deviceAddress)
//{
//	for (uint8_t i = 0; i < 8; i++)
//	{
//		if (deviceAddress[i] < 16) Serial.print("0");
//		Serial.print(deviceAddress[i], HEX);
//	}
//}
////!TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY !TEMPERATURY 
//
