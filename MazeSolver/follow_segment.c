#include <pololu/3pi.h>


void follow_segment(void)
{
	int last_proportional = 0;
	long integral = 0;
	
	while(1)
	{
		unsigned int sensors[5];
		unsigned int position;
		int proportional, derivative;
		int power_difference;
		const int max = 60;
		
		position = read_line(sensors, IR_EMITTERS_ON);
		proportional = ((int)position) - 2000;
		derivative = proportional - last_proportional;
		integral += proportional;
		last_proportional = proportional;
		
		power_difference = proportional/20 + integral/10000 + derivative*3/2;
		
		if (power_difference > max)
		{
			power_difference = max;
		}
		else if (power_difference < -max)
		{
			power_difference = -max;
		}
		
		if (power_difference < 0)
		{
			set_motors(max+power_difference, max);
		}
		else
		{
			set_motors(max, max-power_difference);
		}
		
		if (sensors[1] < 100 && sensors[2] < 100 && sensors[3] < 100)
		{
			// no line ahead
			break;
		}
		if (sensors[0] > 200 || sensors[4] > 200)
		{
			// intersection
			break;
		}
	
	}

}