#include <pololu/orangutan.h>
#include <pololu/3pi.h>
#include <avr/pgmspace.h>

const char welcome_line1[] PROGMEM = " Pololu";
const char welcome_line2[] PROGMEM = "3\xf7 Robot";
const char demo_name_line1[] PROGMEM = "\xf7 Line \xf7"; // change to note
const char demo_name_line2[] PROGMEM = "Musician";

const char welcome[] PROGMEM = ">g32>>c32";
const char go[] PROGMEM = "L16 cdegreg4";

#define DELAY_IN_MS	100

void initialize()
{
	// This must be called at the beginning of 3pi code, to set up the
	// sensors.  We use a value of 2000 for the timeout, which
	// corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
	pololu_3pi_init(2000);
	
	// Play welcome music and display a message
	print_from_program_space(welcome_line1);
	lcd_goto_xy(0,1);
	print_from_program_space(welcome_line2);
	play_from_program_space(welcome);
	delay_ms(1000);

	clear();
	print_from_program_space(demo_name_line1);
	lcd_goto_xy(0,1);
	print_from_program_space(demo_name_line2);
	delay_ms(2000);

	// Display battery voltage and wait for button press
	while(!button_is_pressed(BUTTON_B))
	{
		int bat = read_battery_millivolts();

		clear();
		print_long(bat);
		print("mV");
		lcd_goto_xy(0,1);
		print("Press B");

		delay_ms(100);
	}

	// Always wait for the button to be released so that 3pi doesn't
	// start moving until your hand is away from it.
	wait_for_button_release(BUTTON_B);
	delay_ms(1000);
	
	{
		unsigned int sensors[5]; // an array to hold sensor values
		int counter;
		for(counter=0;counter<80;counter++)
		{
			if(counter < 20 || counter >= 60)
				set_motors(40,-40);
			else
				set_motors(-40,40);

		// This function records a set of sensor readings and keeps
		// track of the minimum and maximum values encountered.  The
		// IR_EMITTERS_ON argument means that the IR LEDs will be
		// turned on during the reading, which is usually what you
		// want.
			calibrate_line_sensors(IR_EMITTERS_ON);

		// Since our counter runs to 80, the total delay will be
		// 80*20 = 1600 ms.
			delay_ms(20);
		}
		set_motors(0,0);
	}
}

int should_stop(int index)
{
	unsigned int sensors[5];
	read_line(sensors, IR_EMITTERS_ON);
	return sensors[0] > 200 && sensors[4] > 200;
}

int simple_player(int index)
{
	unsigned int sensors[5];
	set_motors(50, 50);
	do
	{
		read_line(sensors, IR_EMITTERS_ON);
		if (sensors[2] > 100)
		{
			play_note(E(5), DELAY_IN_MS, 15);
		}
		else
		{
			stop_playing();
		}
		delay_ms(DELAY_IN_MS);
	}
	while(!should_stop(index));
	set_motors(0, 0);
	stop_playing();
	return 1;
}

typedef int (*prog_func)(int);
static prog_func prog_funcs[] = 
{
	&simple_player,
};
#define NUMBER_OF_FUNCS	(sizeof(prog_funcs) / sizeof(prog_funcs[0]))

int choose_program()
{
	clear();
	while(!button_is_pressed(BUTTON_B))
	{
		lcd_goto_xy(0,1);
		print("Press B");
		delay_ms(100);
	}
	wait_for_button_release(BUTTON_B);
	delay_ms(1000);
	return 0;
}

void run_program(unsigned int program)
{
	
	if (program >= NUMBER_OF_FUNCS)
	{
		// error
		return;
	}
	(*prog_funcs[program])(program);
}

int main()
{
	int chosen_program;
	// set up the 3pi
	initialize();
	
	while(1)
	{
		chosen_program = choose_program();
		run_program(chosen_program);
	}
}