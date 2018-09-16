#include "ErrorHandlerPrinter.h"




ErrorHandlerPrinter::ErrorHandlerPrinter(LiquidCrystal_I2C & lcd, ErrorHandler & worker)
	:Printer(lcd)
{
	this->worker = &worker;
}

void ErrorHandlerPrinter::InitPrint()
{
	this->lcd->clear();
	this->lcd->setCursor(2, 0);
	this->lcd->print("ODNOTOWANO BLAD");
	this->lcd->setCursor(0, 1);
	this->lcd->print("Kod bledu = ");
	this->lcd->print(worker->errorCode);
	this->lcd->setCursor(0, 2);
	this->lcd->print(worker->errorMessage1);
	this->lcd->setCursor(0, 3);
	this->lcd->print(worker->errorMessage2);
}

void ErrorHandlerPrinter::AskPrint()
{
	this->lcd->clear();
	this->lcd->setCursor(0, 0);
	this->lcd->print("  SKASOWAC BLAD?");
	this->lcd->setCursor(0, 1);
	this->lcd->print("Kod bledu = ");
	this->lcd->print(worker->errorCode);
	this->lcd->setCursor(0, 2);
	this->lcd->print(" PRZYCISK 2 = NIE");
	this->lcd->setCursor(0, 3);
	this->lcd->print(" PRZYCISK 3 = TAK");
}

void ErrorHandlerPrinter::GoodbayPrint()
{
	this->lcd->clear();
	this->lcd->setCursor(0, 1);
	this->lcd->print("   BLAD ZOSTAL");
	this->lcd->setCursor(0, 2);
	this->lcd->print("     SKASOWANY");
	delay(1000);
}


void ErrorHandlerPrinter::Print()
{
}
