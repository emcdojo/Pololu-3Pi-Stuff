#include <pololu/3pi.h>
#include "turn.h"
#define NINETY_DEGREES_TURN_TIMING	200
#define HUNDRED_AND_EIGHTY_TURN_TIMING (NINETY_DEGREES_TURN_TIMING * 2)

unsigned char select_turn(unsigned char found)
{
	if (found & FOUND_LEFT)
	{
		return 'L';
	}
	if (found & FOUND_STRAIGHT)
	{
		return 'S';
	}
	if (found & FOUND_RIGHT)
	{
		return 'R';
	}
	return 'B';
}

void turn(char direction)
{
	switch (direction)
	{
		// Turn left
		case 'L':
			set_motors(-80,80);
			delay_ms(NINETY_DEGREES_TURN_TIMING);
			break;
		// Turn right
		case 'R':
			set_motors(80, -80);
			delay_ms(NINETY_DEGREES_TURN_TIMING);
			break;
		// Turn around
		case 'B':
			set_motors(80, -80);
			delay_ms(HUNDRED_AND_EIGHTY_TURN_TIMING);
			break;
		// Keep straight
		default:
			break;			
	}
}