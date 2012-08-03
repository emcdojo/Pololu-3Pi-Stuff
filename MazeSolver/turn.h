#ifndef TURN_H
#define TURN_H

#define FOUND_RIGHT 	1
#define FOUND_LEFT 		2
#define FOUND_STRAIGHT	4

void turn(char directrion);
unsigned char select_turn(unsigned char found);

#endif //TURN_H