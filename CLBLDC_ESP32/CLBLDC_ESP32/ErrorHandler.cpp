#include "ErrorHandler.h"



ErrorHandler::ErrorHandler(InputOutput &IO, int motorNPin, int motorSPin, int hallNPin, int hallSPin, int pwmCh1, int pwmCh2, int pwmFreq, int pwmRes)
	:MovementMode(motorNPin, motorSPin, hallNPin, hallSPin, pwmCh1, pwmCh2, pwmFreq, pwmRes)
	{
		this->IO = &IO;
		Serial.println("ErrorHandler configurated sucessfully.");
	}


void ErrorHandler::Alarm()
{
	static bool status;
	static unsigned int LastChange;
	if (LastChange + 900 < IO->GetNowTime() ) {
		LastChange = millis();
		status = !status;
	}
	//digitalWrite(2, status);
}

bool ErrorHandler::Monitor()
{
	if (IO->GetAmps(100) > maxCurrent) {
		ERROR = true;
		errorCode = 0;
		errorMessage1 = " Przekroczony max ";
		errorMessage2 = " prad";
		return true;
	}
	else return false;
}

bool ErrorHandler::GetStatus()
{
	return ERROR;
}

void ErrorHandler::hallNIRQ()
{

}

void ErrorHandler::hallSIRQ()
{
}

void ErrorHandler::Work()
{
	ledcWrite(pwmCh2, 0);
	ledcWrite(pwmCh1, 0);
	Alarm();
	Monitor();
}

MovementTypes ErrorHandler::GetType()
{
	return Error;
}

void ErrorHandler::setParameter(bool upordown)
{
	if (upordown && deletingError)
	{
		ERROR = false;
		digitalWrite(2, LOW);
	}
	else 
	{
		ERROR = true;
	}
	deletingError = false;
}

void ErrorHandler::reset()
{
}
