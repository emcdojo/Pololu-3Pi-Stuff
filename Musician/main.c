#include <pololu/orangutan.h>
#include <pololu/3pi.h>
#include <avr/pgmspace.h>

const char welcome_line1[] PROGMEM = " Pololu";
const char welcome_line2[] PROGMEM = "3\xf7 Robot";
const char demo_name_line1[] PROGMEM = " Line "; // change to note
const char demo_name_line2[] PROGMEM = "Musician";

#define NOTE_SYMBOL_ID		1
const char note_symbol[] PROGMEM = {
	0b00110,
	0b00110,
	0b00101,
	0b00101,
	0b01100,
	0b11100,
	0b11100,
	0b01100,
};

const char welcome[] PROGMEM = ">g32>>c32";

#define DELAY_IN_MS	100

void initialize()
{
	// This must be called at the beginning of 3pi code, to set up the
	// sensors.  We use a value of 2000 for the timeout, which
	// corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
	pololu_3pi_init(2000);
	
	lcd_load_custom_character(note_symbol, NOTE_SYMBOL_ID);
	clear();
	
	// Play welcome music and display a message
	print_from_program_space(welcome_line1);
	lcd_goto_xy(0,1);
	print_from_program_space(welcome_line2);
	play_from_program_space(welcome);
	delay_ms(1000);

	clear();
	lcd_goto_xy(0,0);
	print_character(NOTE_SYMBOL_ID);
	lcd_goto_xy(1,0);
	print_from_program_space(demo_name_line1);
	lcd_goto_xy(7,0);
	print_character(NOTE_SYMBOL_ID);
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

int dummy_function(int index)
{
	clear();
	print("Dummy");
	delay_ms(2000);
	return 0;
}	

typedef int (*prog_func)(int);
static prog_func prog_funcs[] = 
{
	&simple_player,
	&dummy_function,
};
static const char * program_names[] =
{
	"Simple",
	"Dummy",
};
#define NUMBER_OF_FUNCS	(sizeof(prog_funcs) / sizeof(prog_funcs[0]))

void show_program_menu(int current)
{
	clear();
	lcd_goto_xy(0,0);
	print_character(current + '1');
	lcd_goto_xy(1,0);
	print_character('.');
	lcd_goto_xy(2,0);
	print(program_names[current]);
	if (current > 0)
	{
		lcd_goto_xy(0,1);
		print_character('-');
	}
	lcd_goto_xy(3,1);
	print("Go");
	if (current < (NUMBER_OF_FUNCS - 1))
	{
		lcd_goto_xy(7,1);
		print_character('+');
	}
}

unsigned char pause(void) 
{
	unsigned char button;
	button = wait_for_button_press(BUTTON_A | BUTTON_B | BUTTON_C);
	wait_for_button_release(button);
	return button;
}

int choose_program()
{
	int current = 0;
	while(1) {
		unsigned char button;
		show_program_menu(current);
		button = pause();
		if (button & BUTTON_A)
		{
			if (current > 0)	--current;
		}
		else if (button & BUTTON_C)
		{
			if (current < (NUMBER_OF_FUNCS - 1)) ++current;
		}		
		else if (button & BUTTON_B)
		{
			break;
		}
	} 
	return current;
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
		delay_ms(1000);
		run_program(chosen_program);
	}
}