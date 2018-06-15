/*
 * author: Daniel Raab
 * date: 12.06.18
 * version: 2.0
 *
 * library to detect the inputs of the buttons and the encoder and react by setting the corresponding flags
 * currently still uses delay cycles as a means to debounce the buttons, but it works fine in the full project,
 * so for the time being there is no need to optimise
 *
 * Usage:
 * initialise by calling
 * input_init();
 * and then, when the inputs should be read, enable global interrupts with
 * __enable_interrupt();
 * 
 * the processor is woken up from low-power mode after a input was received
 * then the flags should be read and reacted upon
 */


#ifndef BUTTON_H_
#define BUTTON_H_

#include <msp430.h>
#include <stdbool.h>

// the flags that are set when button presses and encoder turns were detected
bool button_SW1;
bool button_SW2;
bool button_SW3;
bool button_SW4;
bool encoder_l;
bool encoder_r;

// the ISR for the buttons and the encoder
__interrupt void P2_ISR();

void input_init(); // initialisation function
void debounce_delay(); // debouncing function to keep the system from detecting one button press as many

#endif /* BUTTON_H_ */
