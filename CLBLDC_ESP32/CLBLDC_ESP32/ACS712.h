// ACS712.h

#ifndef _ACS712_h
#define _ACS712_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class ACS712 {
private:
	int analogPin;
	int mvPerAmp;
	int baseVoltage;
	int ACSoffset;
	double zeroAmps;
	int RawValue;
	int resolution;
	double Voltage;
	double Amps;

	void intro();
public:
	//ACS712(int analogPin, int mvPerAmp, int baseVoltage, int resolution);
	ACS712(int analogPin, int mvPerAmp, int baseVoltage, int resolution);
	ACS712(int mvPerAmp, int baseVoltage, int resolution);
	double getAmps(int samples);

	void Setupb(int samplesToOffset, int samplesToZeroAmp);

	void SetPin(int pin);
};


#endif

