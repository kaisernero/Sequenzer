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

#ifndef LCD_H_
#define LCD_H_

#include <msp430.h>
#include <stdbool.h>

#define LCD_DATA_OUT 	P1OUT
#define LCD_DATA_DIR	P1DIR
#define LCD_COM_OUT		P3OUT
#define LCD_COM_DIR 	P3DIR

#define RS_BIT    		( 0x0001 ) // RS Display
#define ENABLE_BIT		( 0x0008 ) // Enable Display

#define SELECT_COMMAND_REG   LCD_COM_OUT &= ~RS_BIT // set RS low
#define SELECT_DATA_REG      LCD_COM_OUT |=  RS_BIT // set RS high

#define DISPLAY_ENABLE_HIGH 	LCD_COM_OUT |=  ENABLE_BIT
#define DISPLAY_ENABLE_LOW  	LCD_COM_OUT &= ~ENABLE_BIT


unsigned int cursor_x, cursor_y;

void lcd_clear();
void lcd_delay();
void lcd_set_cursor(unsigned char x, unsigned char y);
void lcd_show_cursor(bool visible, bool blinking);
void lcd_init();
void lcd_send_command(unsigned char command);
void lcd_send_char(unsigned char byte);
void lcd_write(unsigned char character);
void lcd_write_string(unsigned char string[]);
void lcd_write_int(unsigned int number, int digits);

#endif
