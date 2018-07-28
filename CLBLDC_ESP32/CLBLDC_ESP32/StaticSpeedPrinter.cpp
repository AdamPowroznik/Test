#include "StaticSpeedPrinter.h"



void StaticSpeedPrinter::updateValues()
{
	RPM = worker->SPEED;
	VOLTAGE = worker->VOLTAGE;
	CURRENT = worker->CURRENT;
	POWER = VOLTAGE * CURRENT;
	SIDE = worker->SIDE;
	if (SIDE == 'R')
		SIDE = '>';
	else if (SIDE == 'L')
		SIDE = '<';
	else SIDE = '-';
	PARAMETER = worker->WANTEDSPEED;
	if (worker->WANTEDPWM == 255)
		comment = "*MAKS. MOC*";
	else comment = " ";	
	if (worker->MAINENABLED && worker->WANTEDSPEED < 300)
		comment = "*MIN 300 RPM*";
	else comment = " ";
}

StaticSpeedPrinter::StaticSpeedPrinter(LiquidCrystal_I2C &lcd, StaticSpeed &worker, String name, int maxSpeed) :Printer(lcd)
{
	this->name = name;
	this->worker = &worker;
	this->maxSpeed = maxSpeed;
	updateValues();
}

StaticSpeedPrinter::~StaticSpeedPrinter()
{
}

void StaticSpeedPrinter::InitPrint() {
	updateValues();

	this->lcd->setCursor(0, 0);
	this->lcd->print("     ");
	this->lcd->setCursor(0, 0);
	this->lcd->print(RPM);
	this->lcd->print(SIDE);
	this->lcd->setCursor(5, 0);
	this->lcd->print("| ");
	this->lcd->print(name);

	this->lcd->setCursor(0, 1);
	this->lcd->print("     ");
	this->lcd->setCursor(0, 1);
	this->lcd->print(VOLTAGE, 1);
	this->lcd->setCursor(5, 1);
	this->lcd->print("|");
	this->lcd->print(title);

	this->lcd->setCursor(0, 2);
	this->lcd->print("            ");
	this->lcd->setCursor(0, 2);
	this->lcd->print(CURRENT);
	this->lcd->setCursor(5, 2);
	this->lcd->print("| ");
	this->lcd->print(PARAMETER, 0);
	this->lcd->print("/");
	this->lcd->print(maxSpeed);

	this->lcd->setCursor(0, 3);
	this->lcd->print("     ");
	this->lcd->setCursor(0, 3);
	this->lcd->print(POWER, 2);
	this->lcd->setCursor(5, 3);
	this->lcd->print("| ");
	this->lcd->print(comment);
}

void StaticSpeedPrinter::Print()
{
	static int lastPrint;
	if (lastPrint + interval < worker->NOW) {
		lastPrint = millis();
		//lcd->clear();

		updateValues();

		this->lcd->setCursor(0, 0);
		this->lcd->print("     ");
		this->lcd->setCursor(0, 0);
		this->lcd->print(RPM);
		this->lcd->print(SIDE);
		this->lcd->setCursor(0, 1);
		this->lcd->print("     ");
		this->lcd->setCursor(0, 1);
		this->lcd->print(VOLTAGE, 1);


		this->lcd->setCursor(0, 2);
		this->lcd->print("            ");
		this->lcd->setCursor(0, 2);
		this->lcd->print(CURRENT);
		this->lcd->setCursor(7, 2);
		this->lcd->print("         ");
		this->lcd->setCursor(5, 2);
		this->lcd->print("| ");
		this->lcd->print(PARAMETER,0);
		this->lcd->print("/");
		this->lcd->print(maxSpeed);

		this->lcd->setCursor(0, 3);
		this->lcd->print("     ");
		this->lcd->setCursor(0, 3);
		this->lcd->print(POWER, 2);
		this->lcd->setCursor(6, 3);
		this->lcd->print("              ");
		this->lcd->setCursor(5, 3);
		this->lcd->print("|");
		this->lcd->print(comment);
	}

}
