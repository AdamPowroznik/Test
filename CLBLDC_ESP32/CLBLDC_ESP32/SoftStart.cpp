#include "SoftStart.h"


int SoftStart::setPwm(int lastPwm, int futurePwm)
{
	int pwmToReturn = lastPwm;
	static unsigned long last_change_time = 0;
	unsigned long this_time = millis();
	if (this_time - last_change_time > 10)
	{
		if (futurePwm > lastPwm) {
			pwmToReturn++;
			if (pwmToReturn > 255)
				pwmToReturn = 255;
			return pwmToReturn;
		}
		else if (futurePwm < lastPwm) {
			pwmToReturn--;
			if (pwmToReturn < 75)
				pwmToReturn = 75;
			return pwmToReturn;
		}
		last_change_time = this_time;
	}
}


int SoftStart::GetPwmSoft(int lastPwm, int futurePwm)
{
	return setPwm(lastPwm, futurePwm);
}

