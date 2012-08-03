#include <pololu/3pi.h>
#include "turn.h"
#include "follow_segment.h"

void display_path(char * path, unsigned char path_length)
{
	// Set the last character of the path to a 0 so that the print()
	// function can find the end of the string.  This is how strings
	// are normally terminated in C.
	path[path_length] = 0;

	clear();
	print(path);

	if(path_length > 8)
	{
		lcd_goto_xy(0,1);
		print(path+8);
	}
}

unsigned char simplify_path(char *path, unsigned char path_length)
{
	int total_angle = 0;
	int i;
	if (path_length < 3 || path[path_length -2] != 'B')
	{
		return path_length;
	}
	
	for (i = 1; i <= 3; ++i)
	{
		switch(path[path_length-i])
		{
			case 'L':
				total_angle += 270;
				break;
			case 'B':
				total_angle += 180;
				break;
			case 'R':
				total_angle += 90;
				break;
		}
	}
	total_angle = total_angle % 360;
	switch(total_angle)
	{
		case 0:
			path[path_length -3] = 'S';
			break;
		case 90:
			path[path_length -3] = 'R';
			break;
		case 180:
			path[path_length -3] = 'B';
			break;
		case 270:
			path[path_length -3] = 'L';
			break;
	}
	
	return path_length - 2;
}

void maze_solve()
{
	char path[100] = "";
	unsigned char path_length = 0;

	while (1)
	{
		unsigned char found = 0;
		unsigned char dir;
		unsigned int sensors[5];
		unsigned char old_length;
		
		follow_segment();
		set_motors(50, 50);
		delay_ms(50);
		
		read_line(sensors, IR_EMITTERS_ON);
		
		if (sensors[0] > 100)
		{
			found |= FOUND_LEFT;
		}
		if (sensors[4] > 100)
		{
			found |= FOUND_RIGHT;
		}
		
		set_motors(40, 40);
		delay_ms(200);
		
		read_line(sensors, IR_EMITTERS_ON);
		if (sensors[1] > 200 || sensors[2] > 200 || sensors[3] > 200)
		{
			found |= FOUND_STRAIGHT;
		}
		
		if (sensors[1] > 600 && sensors[2] > 600 && sensors[3] > 600)
		{
			break;
		}
		
		dir = select_turn(found);
		turn(dir);
		path[path_length] = dir;
		++path_length;
		
		do 
		{
			old_length = path_length;
			path_length = simplify_path(path, path_length);
		} while(old_length != path_length);
		display_path(path, path_length);

	}
	
	while (1)
	{
		int i;
		// Beep to show that we finished the maze.
		set_motors(0,0);
		play(">>a32");

		// Wait for the user to press a button, while displaying
		// the solution.
		while(!button_is_pressed(BUTTON_B))
		{
			if(get_ms() % 2000 < 1000)
			{
				clear();
				print("Solved!");
				lcd_goto_xy(0,1);
				print("Press B");
			}
			else
				display_path(path, path_length);
			delay_ms(30);
		}
		while(button_is_pressed(BUTTON_B));
	
		delay_ms(1000);
		
		for (i = 0; i < path_length; ++i)
		{
			follow_segment();
			set_motors(50,50);
			delay_ms(50);
			set_motors(40,40);
			delay_ms(200);
			
			turn(path[i]);
		}
		follow_segment();
	}
}
