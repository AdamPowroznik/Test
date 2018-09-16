#pragma once
#include "Printer.h"
#include "ErrorHandler.h"
class ErrorHandlerPrinter :
	public Printer
{
public:
	ErrorHandlerPrinter(LiquidCrystal_I2C &lcd, ErrorHandler &worker);


	ErrorHandler *worker;
	void InitPrint();
	void AskPrint();
	void GoodbayPrint();

	virtual void Print();
};

