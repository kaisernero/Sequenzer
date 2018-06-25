/*
 * led_matrix.h
 *
 *  Created on: 25.06.2018
 *      Author: draab
 */

#ifndef LED_MATRIX_H_
#define LED_MATRIX_H_

/*
 * register(in the MCP23008) for the led-matrix: GPIO
 * GPIO address is 0x09
 * i2c adress of pin expander: 0x20
 */

#define I2C_EXPANDER_ADR 0x20
#define EXPANDER_IO_DIR 0x00
#define EXPANDER_LED_OUT 0x09

void i2c_init();
void i2c_write_byte(unsigned char i2c_address, unsigned char expander_reg, unsigned char data);
void led_on(unsigned char led_nr);

#endif /* LED_MATRIX_H_ */
