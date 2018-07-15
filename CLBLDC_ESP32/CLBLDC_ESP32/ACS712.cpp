// 
// 
// 
#include "ACS712.h"



//#if defined(ARDUINO) && ARDUINO >= 100
//#include "arduino.h"
//#else
//#include "WProgram.h"
//#endif


//#include "ACS712.h"
//class ACS712 {
//private:
		int analogPin;
		int mvPerAmp;
		int baseVoltage;
		int ACSoffset;
		double zeroAmps;
		int RawValue = 0;
		int resolution;
		double Voltage;
		double Amps;
	
	//public:
		//ACS712(int analogPin, int mvPerAmp, int baseVoltage, int resolution) {
		//	this->analogPin = analogPin;
		//	this->mvPerAmp = mvPerAmp;
		//	this->baseVoltage = baseVoltage;
		//	this->resolution = resolution;
		//}
	
		void ACS712::intro()
		{
			Serial.print("ACS IO intro: analogPin ");
			Serial.println(analogPin);
			Serial.print("	mvPerAmp ");
			Serial.println(mvPerAmp);
			Serial.print("	baseVoltage ");
			Serial.println(baseVoltage);
			Serial.print("	ACSoffset ");
			Serial.println(ACSoffset);
			Serial.print("	zeroAmps ");
			Serial.println(zeroAmps);
			Serial.print("	RawValue ");
			Serial.println(RawValue);
			Serial.print("	resolution ");
			Serial.println(resolution);
			Serial.print("	Voltage ");
			Serial.println(Voltage);
			Serial.print("	Amps ");
			Serial.println(Amps);
		}

		ACS712::ACS712(int analogPin, int mvPerAmp, int baseVoltage, int resolution)
		{
			this->analogPin = analogPin;
			this->mvPerAmp = mvPerAmp;
			this->baseVoltage = baseVoltage;
			this->resolution = resolution;
			pinMode(analogPin, INPUT);
			Serial.print("New Current Meter created on pin ");
			Serial.println(analogPin);
			Setupb(10000, 100000);
		}

		ACS712::ACS712(int mvPerAmp, int baseVoltage, int resolution)
		{
			this->mvPerAmp = mvPerAmp;
			this->baseVoltage = baseVoltage;
			this->resolution = resolution;
			pinMode(analogPin, INPUT);
			Serial.println("New CurrentMeter created without assigned pin");
			Setupb(10000, 100000);
		}

		double ACS712::getAmps(int samples) {
			double amp;
			for (int i = 0; i<samples; i++)
			{
				RawValue = analogRead(analogPin);
				Voltage = (RawValue / pow(2, resolution)) * baseVoltage;
				amp += (((Voltage - ACSoffset) / mvPerAmp) -zeroAmps);
			}
			amp /= samples;
			Amps = amp;
			//intro();
			return amp;
		}
	
		void ACS712::Setupb(int samplesToOffset, int samplesToZeroAmp) {
			Serial.println("Current meter basing...");
			for (int i = 0; i<samplesToOffset; i++) {
				ACSoffset += analogRead(analogPin);
			}
			ACSoffset /= samplesToOffset;
	
			for (int i = 0; i<samplesToZeroAmp; i++)
			{
				RawValue = analogRead(analogPin);
				double zeroVoltage = (RawValue / pow(2, resolution)) * baseVoltage;
				zeroAmps += ((zeroVoltage - ACSoffset) / mvPerAmp);
			}
			zeroAmps /= samplesToZeroAmp;
			Serial.print("Current meter on pin ");
			Serial.print(analogPin);
			Serial.println (" based");
		}

		void ACS712::SetPin(int pin)
		{
			this->analogPin = pin;
		}

