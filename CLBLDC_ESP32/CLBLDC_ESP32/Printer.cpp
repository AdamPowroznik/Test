#include "Printer.h"



Printer::Printer(LiquidCrystal_I2C &lcd)
{
	this->lcd = &lcd;
	this->lcd->begin();
	this->lcd->clear();
	this->lcd->setCursor(4, 1);
	this->lcd->print("LCD I2C 4x20");
	this->lcd->setCursor(0, 2);
	this->lcd->print("Configured sucessful");
	delay(1000);
}

Printer::~Printer()
{
}

void Printer::SetInterval(int value)
{
	this->interval = value;
}
