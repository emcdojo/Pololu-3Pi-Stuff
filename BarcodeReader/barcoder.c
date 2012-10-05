/*************************************************************************
barcoder.c
one day this robot hopefully read an EAN13 barcode with its sensors and 
decode it. Currently I only integrated the enc-dec algorithm in a simple
program:
1. Read a 13-digit number from user (using buttons)
2. Encode the number and print the result
3. Decode it back and print the result
*************************************************************************/

#include <pololu/orangutan.h>
#include <pololu/3pi.h>
#include <avr/pgmspace.h>
#include "Ean13.h"
#include <string.h>
const char welcome_line1[] PROGMEM = " Pololu";
const char welcome_line2[] PROGMEM = "3\xf7 Robot";
const char demo_name_line1[] PROGMEM = "EAN-13"; // change to note
const char demo_name_line2[] PROGMEM = "EncDec";

const char welcome[] PROGMEM = ">g32>>c32";

void initialize()
{
	// This must be called at the beginning of 3pi code, to set up the
	// sensors.  We use a value of 2000 for the timeout, which
	// corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
	pololu_3pi_init(2000);
	
	clear();
	
	// Play welcome music and display a message
	print_from_program_space(welcome_line1);
	lcd_goto_xy(0,1);
	print_from_program_space(welcome_line2);
	play_from_program_space(welcome);
	delay_ms(1000);

	clear();
	lcd_goto_xy(1,0);
	print_from_program_space(demo_name_line1);
	lcd_goto_xy(1,1);
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
	
}

unsigned char pause(void) 
{
	unsigned char button;
	button = wait_for_button_press(BUTTON_A | BUTTON_B | BUTTON_C);
	wait_for_button_release(button);
	return button;
}

/*************************************************************************
Print one encoded digit, 1 - black line, 0 - white line
*************************************************************************/
void print_ean13_char(char c)
{
	char output[8] = { 0 };
	int i;
	for (i = 0; i < 7; ++i)
	{
		output[6-i] = ((c >> i) & 1) ? '1' : '0';
	}
	lcd_goto_xy(0,1);
	print(output);
}

/*************************************************************************
Display a full Ean13 encoded buffer
Navigation -	Button A - previous digit
				Button C - Next digit
				Button B - leave
*************************************************************************/
void display_encoded_buffer(const char * encoded_data)
{
	unsigned char button;
	int char_to_show = 0;
	
	clear();
	print("Encoded:");
	
	do
	{
		print_ean13_char(encoded_data[char_to_show]);
		button = pause();
		if (button & BUTTON_A) // go to previous encoded digit
		{
			if (char_to_show > 0)
				char_to_show--;
		}
		else if (button & BUTTON_C) // go to next encoded digit
		{
			if (char_to_show < (ENCODED_NUMBER_SIZE-1))
				char_to_show++;
		}
	}while(!(button & BUTTON_B)); // get our
}
/*************************************************************************
Get a 13 digit number to be encoded
Navigation -	Button A - Increment digit (mod10)
				Button C - Next digit (cyclic)
				Button B - leave
*************************************************************************/
void get_number_to_encode(char * buff)
{
	unsigned char button;
	int current = 0;
	int i;
	
	memset(buff, 0, DECODED_BUFFER_SIZE);
		
	clear();
	print("Encoded:");
	
	do
	{
		// print current digit
		clear();
		print("Digit:");
		print_long(current);
		lcd_goto_xy(0,1);
		print("value:");
		print_long(buff[current]);
		
		button = pause();
		if (button & BUTTON_A) // increment digit value (cyclic)
		{
			buff[current] = (buff[current] + 1) % 10;
		}
		else if (button & BUTTON_C) //go to next digit (cyclic)
		{
			current = (current + 1) % DECODED_NUMBER_SIZE;
		}
	}while(!(button & BUTTON_B)); // done
	
	for (i = 0; i < DECODED_NUMBER_SIZE; ++i) // to ascii
	{
		buff[i] += '0';
	}
	
	// done, show number
	clear();
	print(buff);
	lcd_goto_xy(0,1);
	print(&buff[8]);
	pause();
}

int main()
{
	// set up the 3pi
	initialize();
	
	while(1)
	{
		char encoded[ENCODED_BUFFER_SIZE] = { 0 };
		char decoded[DECODED_BUFFER_SIZE] = { 0 };
		
		// get number
		get_number_to_encode(decoded);
		
		// show encoded
		Ean13_Encode(decoded, encoded);
		display_encoded_buffer(encoded);
		
		// show decoded
		Ean13_Decode(encoded, decoded);
		clear();
		print(decoded);
		lcd_goto_xy(0,1);
		print(&decoded[8]);
		pause();
	}
}