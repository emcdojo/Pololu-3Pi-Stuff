#include <pololu/orangutan.h>
#include <pololu/3pi.h>

#define MIN_POINTS	1
#define MAX_POINTS	10

unsigned char pause(void) 
{
	unsigned char button;
	button = wait_for_button_press(BUTTON_A | BUTTON_B | BUTTON_C);
	wait_for_button_release(button);
	return button;
}

void welcome(void) 
{
	clear();
	print("Collect");
	lcd_goto_xy(0,1);
	print("Points");
	delay_ms(2000);
}

void show_points_menu(int points) 
{
	clear();
	print("How Many");
	lcd_goto_xy(0,1);
	print_character('-');
	lcd_goto_xy(3,1);
	print_long(points);
	lcd_goto_xy(7,1);
	print_character('+');
}

int get_number_of_points(void) 
{
	int points = 5;
	show_points_menu(points);
	while(1) 
	{
		char refresh = 0;
		unsigned char button;
		button = pause();
		if (button & BUTTON_A) 
		{
			if (points > MIN_POINTS) 
			{
				--points;
				refresh = 1;
			}
		}
		else if (button & BUTTON_B) 
		{
			break;
		}
		else 
		{ 
			if (points < MAX_POINTS) 
			{
				++points;
				refresh = 1;
			}
		}
		if (refresh) 
		{
			show_points_menu(points);
		}
	}
	
	return points;
}

void print_left(int p) 
{
	clear();
	print("Points:");
	lcd_goto_xy(0,1);
	print_long(p);
	delay_ms(100);
}

void battary_status(void)
{
	unsigned int bat;
	bat = read_battery_millivolts();
	clear();
	print_long(bat);
	print("mV");
	pause();
}

int init(void) 
{
	int points;
	welcome();
	battary_status();
	points = get_number_of_points();
	print_left(points);
	return points;
}

/* control movement */
void calibrate(void) 
{
	unsigned int counter; // Our timer
	for (counter = 0; counter < 80; ++counter) 
	{
		if (counter < 20 || counter >= 60) 
		{
			set_motors(40, -40);
		}
		else 
		{
			set_motors(-40, 40);
		}
		calibrate_line_sensors(IR_EMITTERS_ON);
		// will make total delay 1600 ms (counter max * 20 ms delay)
		delay_ms(20);		
	}
	set_motors(0,0);
}

void get_calibrated_values(unsigned int position, unsigned int * left, unsigned int * right) 
{
	double dposition = position;
	if (dposition > 2400) 
	{
		dposition = 4000 - dposition;
        	*right = (dposition / 50) * 3; // set it to a range of 0-127
        	*left = 120; 
    	}
    	else if (dposition < 1600)
	{
        	*right = 120;
        	*left = (dposition / 50) * 3; // set it to a range of 0-127
    	}
	else 
	{
		*right = 120;
		*left = 120;
	}
}

int stop_detected(unsigned int * sensors) 
{
	int total = 0;
	total += sensors[0];
	total += sensors[1];
	total += sensors[3];
	total += sensors[4];
	return total > 2700;
	//int i;
	//for (i = 0; i < 5; ++i)
	//	total += sensors[i];
	//return total > 4500;
}


void make_stop(void) 
{
	int i;
	int total_delay = 2000;
	set_motors(0, 0);
	clear();
	print("gotcha");
	for (i = 0; (i * 500) < total_delay; i++) 
	{
		green_led(i % 2);
		red_led(!(i % 2));
		play_note((i % 2) ? C(5): E(5), 500, 20);
		delay_ms(500);
	}
	red_led(0);
	green_led(0);
}

void find_next_stop(void) 
{
	unsigned int sensors[5];
	unsigned int position;
	unsigned int left, right;
	while (1) 
	{
		position = read_line(sensors, IR_EMITTERS_ON);
		if (stop_detected(sensors)) {
			set_motors(60, 60);
			do 
			{
				delay_ms(15);
				read_line(sensors, IR_EMITTERS_ON);			
			} while(stop_detected(sensors));
			make_stop();
			break;
		}
		get_calibrated_values(position, &left, &right);
		set_motors(left, right);
	}
}

long start_driving(int points) 
{
	unsigned long start_time, end_time;
	pololu_3pi_init(2000);
	calibrate();
	start_time = get_ms();
	while(points > 0) 
	{
		find_next_stop();
		--points;
		print_left(points);
	}
	end_time = get_ms();
	return end_time - start_time;
}

void show_results(unsigned long res) 
{
	clear();
	print("Done");
	lcd_goto_xy(0,1);
	print_long(res / 1000);
	print_character('.');
	print_long(res % 1000);
	while (1) 
	{
		delay_ms(2000);
	}
}

int main() 
{
	int points;
	unsigned long results;
	points = init();
	results = start_driving(points);
	show_results(results);
	return 0;
}