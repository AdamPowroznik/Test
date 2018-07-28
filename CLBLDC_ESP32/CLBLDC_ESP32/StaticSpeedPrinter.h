#pragma once

#include "Printer.h"
#include "StaticSpeed.h"

class StaticSpeedPrinter :
	public Printer
{
	void updateValues();
public:
	StaticSpeedPrinter(LiquidCrystal_I2C &lcd, StaticSpeed &worker, String name, int maxSpeed);
	~StaticSpeedPrinter();

	void InitPrint();

	int RPM=0;
	double VOLTAGE;
	double CURRENT;
	double POWER;
	char SIDE;
	double PARAMETER;
	int maxSpeed;

	String name;
	String title = "STALA PREDKOSC";
	String parameter = "MOMENT";
	String comment = "BRAK";


	StaticSpeed *worker;

	virtual void Print();
};

