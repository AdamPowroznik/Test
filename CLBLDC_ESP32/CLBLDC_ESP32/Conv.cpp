#include "Conv.h"



Conv::Conv(int speed)
{
	Serial.begin(speed);
	Serial.print("Serial communication began on speed ");
	Serial.println(speed);
}

