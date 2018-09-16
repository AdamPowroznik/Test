#pragma once

#include "Printer.h"
#include "BrakerMode.h"

class BrakerModePrinter :
	public Printer
{

	void updateValues();
public:
	BrakerModePrinter(LiquidCrystal_I2C &lcd, BrakerMode &worker, String name);
	~BrakerModePrinter();

	int RPM;
	double VOLTAGE;
	double CURRENT;
	double POWER;
	char SIDE;
	double PARAMETER;

	String name;
	String title = "HAMULEC";
	String parameter = "MOMENT";
	String comment = "BRAK";


	BrakerMode *worker;

	virtual void Print();
	virtual void InitPrint();
};

