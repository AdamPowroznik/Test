#pragma once

#include "Printer.h"
#include "StaticMomentum.h"

class StaticMomentumPrinter :
	public Printer
{

	void updateValues();
public:
	StaticMomentumPrinter(LiquidCrystal_I2C &lcd, StaticMomentum &worker, String name);
	~StaticMomentumPrinter();

	int RPM;
	double VOLTAGE;
	double CURRENT;
	double POWER;
	char SIDE;
	double PARAMETER;

	String name;
	String title = "STALY MOMENT";
	String parameter = "MOMENT";
	String comment = "BRAK";


	StaticMomentum *worker;

	virtual void Print();
};

