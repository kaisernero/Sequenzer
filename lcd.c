/*
 * name: lcd.c
 * author: Daniel Raab
 * date: 17.05.2018
 * version: 1.2b
 *
 * This is the library file for all of the functions regarding the LC Display
 * on the Education Board used with the MSP430F2274. It is used in 4-bit-mode,
 * so only 4 data bus pins are needed.
 *
 * lcd_init() needs to be called at first, where the display is initliased cleared and the
 * cursor is turned off
 *
 * pin
 *  1 GND - 9   GND
 *  2 +5V - 10  VCC
 *  3 VLC - LCD HEADER Vo
 *  4 RS  - 1   Register Select
 *  5 RD  - 2   Read/write
 *  6 EN  - 3   LCD_Enable
 *  7 D0  - 5   PA0 - data bus  (not used in 4bit-mode)
 *  8 D1  - 6   PA0 - data bus  (not used in 4bit-mode)
 *  9 D2  - 7   PA0 - data bus  (not used in 4bit-mode)
 * 10 D3  - 8   PA0 - data bus  (not used in 4bit-mode)
 * 11 D4  - 5   PA0 - data bus
 * 12 D5  - 6   PA0 - data bus
 * 13 D6  - 7   PA0 - data bus
 * 14 D7  - 8   PA0 - data bus
 */

#include "lcd.h"

unsigned int base_y[2] = {0x80, 0xc0};
//example for 2x16 display TODO: UNDERSTAND AND BETTER NAME
// 1. line 0x00..0x0F + Bit7==0x80
// 2. line 0x40..0x4F + Bit7==0xc0
// BIT7 nessesary for command "Set DD RAM address"

void lcd_clear() {
	lcd_send_command(0x01); // cursor off
	// function display clear

	cursor_x = 0;
	cursor_y = 0;
}

void lcd_delay() {
	// maybe replace by proper timer routine
	// not really necessary, because it is a function used only at the beginning
	// EXCEPT in lcd_send_char
	__delay_cycles(850);
}

void lcd_set_cursor(unsigned char x, unsigned char y) {
	if (y > 1) {
		y = 0;
	}

	cursor_x = x;
	cursor_y = y;

	lcd_send_command(base_y[y] + x);
}

/*
 * sets the cursor to be visible or not, depending on the parameter. this has no
 * effect on the other functions, so writing still works the same.
 * blinking sets the cursor to be blinking or not (of course only when it is visible)
 *
 *
 * the command to the LC display works as follows:
 *
 * bit 7 6 5 4 3 2 1 0
 *     0 0 0 0 1 D C B
 *
 * D - sets entire Display on/off
 * C - cursor on/off
 * B - blinking of cursor on/off
 */
void lcd_show_cursor(bool visible, bool blinking) {
	unsigned char cursor_command = 0b00001100; // this command would switch the cursor off as well as the blinking (if unaltered)

	if (visible) {
		cursor_command += 0b00000010; // switches the cursor on
	}
	if (blinking) {
		cursor_command += 0b00000001; // switches the blinking on
	}

	lcd_send_command(cursor_command); // send the command to the lcd
}

// initialise the display
char init_codes[6] = {0x33, 0x32, 0x28, 0x0F, 0x01, 0x06};
void lcd_init() {
	P1DIR |= 0x000F; // set the data lines as outputs
	P3DIR |= 0x0009; // set control lines as outputs

	// after receiving the supply voltage the lcd-module needs about 15ms to start
	int i;
	for (i = 70; i > 0; i--) {
		lcd_delay();
	}

	DISPLAY_ENABLE_LOW; // WHAT DOES THIS DO?
	lcd_delay();

	// the display controller needs the init code three times in certain intervals
	lcd_send_command(init_codes[0]); // send init code
	lcd_delay(); // wait at least 4.1ms
	lcd_send_command(init_codes[0]); // send init code
	lcd_delay(); // wait at least 100us
	lcd_send_command(init_codes[0]); // send init code

	// set 4-bit communication
	lcd_send_command(init_codes[1]);
	// set character set and rows
	lcd_send_command(init_codes[2]);
	// display On
	lcd_send_command(init_codes[3]);
	// clear display
	lcd_send_command(init_codes[4]);
	// set entry mode
	lcd_send_command(init_codes[5]);

	lcd_show_cursor(false, false);
}

void lcd_send_command(unsigned char command) {
	SELECT_COMMAND_REG;
	lcd_send_char(command);
}

// TODO replace the lcd_delay() calls with timed routine, because of potential problems
void lcd_send_char(unsigned char byte) {
	// high nibble über BIT0..BIT3 senden
	DISPLAY_ENABLE_HIGH;
	LCD_DATA_OUT = byte >> 4;
	lcd_delay();
	DISPLAY_ENABLE_LOW;
	lcd_delay();

	// low nibble über BIT0..BIT3 senden
	DISPLAY_ENABLE_HIGH;
	LCD_DATA_OUT = byte;
	lcd_delay();
	DISPLAY_ENABLE_LOW;
	lcd_delay();
}

void lcd_write(unsigned char character) {
	// check for line overflow
	if (cursor_x >= 16) {
		cursor_y++;
		cursor_x = 0;
		lcd_set_cursor(cursor_x, cursor_y);
	}

	// send character
	SELECT_DATA_REG;
	lcd_send_char(character);
	cursor_x++;
}


/*
 * writes an array of unsigned characters to the LC display
 *
 * uses the fact, that an array is just a pointer. it reads the unsigned char at the position
 * of the pointer, writes it to the display and then increments the pointer to the next position
 * until it encounters the null terminator of the
 * string (0x0 or '\0').
 */
void lcd_write_string(unsigned char string[]) {
	while (*string != '\0') {
		lcd_write(*string);
		string++;
	}
}

/* converts the given number to characters and then writes them to the LC Display (max 16)
 *
 * the modulo operator % yields the remainder of a division, e.g.
 * 13 % 5 = 3 (because 13/5=2 plus the remaining 3)
 *
 * here it is used to get the last digit of a number by calculating the modulo with 10, e.g.
 * 125 % 10 = 5
 * then dividing the number by ten (and rounding because it is an integer number)
 * -> 12
 * 12 % 10 = 2
 * divide it by ten again
 * 1 % 10 = 1
 * this way you get another digit, starting at the end, for every loop
 * this is done as often as the parameter digits says, stored in a string and then written to the LC display
 *
 * to convert to a character the layout of the numbers in the ASCII code is used, as they are all
 * consecutive after the '0' character ('1', '2', and so on) and the value of '0' is just increased by the number
 */
void lcd_write_int(unsigned int number, int digits) {
	unsigned char string[17];
	string[digits] = 0x0;

	int i;
	for (i = digits - 1; i >= 0; i--) {
		string[i] = (unsigned char)(number % 10 + '0');
		number /= 10;

		// TODO: choice to replace '0' in front with ' '
		if (number == 0) {

		}
	}

	lcd_write_string(string);
}
