/*
 * led_matrix.c
 *
 *  Created on: 25.06.2018
 *      Author: draab
 */

#include <msp430.h>
#include "led_matrix.h"

// switches on the led at the given number, with 'line breaks' so 0 to 15, then it repeats again
void led_on(unsigned char led_nr) {
	unsigned char data_byte = 0x00;
	// colum (first nibble): 0 to switch the led on
	data_byte += (0xEF << (led_nr / 4)) & 0xF0;
	// row (second nibble): 1 to switch the led on
	data_byte += 0x01 << (led_nr % 4);

	// send to expander
	i2c_write_byte(I2C_EXPANDER_ADR, EXPANDER_LED_OUT, data_byte);
}

void i2c_init() {
	// data and clock pin setup for i2c
	// SDA P3.1
	// SCL P3.2
	P3DIR |= BIT1 + BIT2;
	P3OUT &= ~(BIT1 + BIT2);

	P3SEL |= BIT1 + BIT2; // select i2c for the pins

	UCB0CTL1 |= UCSWRST;                    // Enable SW reset
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;   // I2C Master, synchronous mode
	UCB0CTL1 = UCSSEL_2 + UCSWRST;          // Use SMCLK, keep SW reset
	UCB0BR0  = 2;    			            // fSCL = SMCLK/12 = ~100kHz
											// fSCL = SMCLK/3  = ~400kHz
											// fSCL = SMCLK/2  = ~600kHz
	UCB0BR1 = 0;
	UCB0I2COA = 0x4e; // own address

	UCB0CTL1 &= ~UCSWRST; // clear SW reset, resume operation

	i2c_write_byte(I2C_EXPANDER_ADR, EXPANDER_IO_DIR, 0x00); // set GPIO pins as outputs
}

void i2c_write_byte(unsigned char i2c_address, unsigned char expander_reg, unsigned char data) {
	// wait for i2c bus
	while(UCB0STAT && UCBBUSY);
	// set i2c address
	UCB0I2CSA = i2c_address;
	// send start 'signal'
	UCB0CTL1 |= UCTR + UCTXSTT;

	UCB0TXBUF = expander_reg; // send i2c device address
	while (!(UCB0TXIFG & IFG2));
	UCB0TXBUF = data;
	while (!(UCB0TXIFG & IFG2));
	UCB0CTL1 |= UCTXSTP;
	IFG2 &= ~UCB0TXIFG;                             // delete IRQ Flag
}
