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

#include "input.h"

// intialises the registers and the flags
void input_init() {
	// interrupt edge select for P2 input pins 0, 1, 2 and 5 (the buttons)
	// 0 selects rising edge (change from 0 to 1)
	// 1 selects falling edge (change from 1 to 0)
	// here the falling edge is chosen for the button pins
	P2IES = BIT0 + BIT1 + BIT2 + BIT5;
	// interrupt enable for the same pins, which enables the pins to cause an interrupt
	P2IE = BIT0 + BIT1 + BIT2 + BIT5;

	// select falling edge for encoder pin
	P2IES |= BIT3;
	// enable interrupt for the encoder pin
	P2IE |= BIT3;

	// give every flag the init value of false
	button_SW1 = false;
	button_SW2 = false;
	button_SW3 = false;
	button_SW4 = false;
	encoder_l = false;
	encoder_r = false;
}

// ISR for port 2
#pragma vector = PORT2_VECTOR
__interrupt void P2_ISR() {
	// check encoder
	if (P2IFG & BIT3) {
		// left turn?
		if ((P4IN & BIT1) && !(P4IN & BIT2)) {
			encoder_l = true;
		}
		// right turn?
		else if (!(P4IN & BIT1) && (P4IN & BIT2)) {
			encoder_r = true;
		}
		// reset interrupt flag
		P2IFG &= ~BIT3;
	}
	else {
		debounce_delay();

		/*
		 * Explanation of the if conditional: P2IFG & BIT0 (e.g.)
		 *
		 * & - the bitwise OR (in contrast to the boolean OR: &&)
		 * So, the two 8-bit numbers (chars) are calculated via the bitwise OR to make one 8-bit number.
		 * This 8-bit number then is used by the if, evaluating to false if it is 0 or true if it is any other number.
		 *
		 * Example:
		 * VARIABLE          | VALUE
		 * ------------------|----------------
		 * P2IFG			 | 0b00110000
		 * BIT5				 | 0b00010000
		 * P2IFG & BIT5		 | 0b00010000
		 * if (P2IFG & BIT5) | true, because it is NOT ZERO
		 *
		 * ELSE IF
		 * the else if is there to prevent hard to debug errors, by only allowing one buttonpress to be detected for every interrupt
		 */

		// check if P2.0 was pressed (SW4)
		if (P2IFG & BIT0) {
			button_SW4 = true; // call button press handler
			P2IFG &= ~BIT0; // reset the interrupt flag
		}
		// check if P2.1 was pressed (SW3)
		else if (P2IFG & BIT1) {
			button_SW3 = true; // call button press handler
			P2IFG &= ~BIT1; // reset the interrupt flag
		}
		// check if P2.2 was pressed (SW2)
		else if (P2IFG & BIT2) {
			button_SW2 = true; // call button press handler
			P2IFG &= ~BIT2; // reset the interrupt flag
		}
		// check if P2.5 was pressed (SW1)
		else if (P2IFG & BIT5) {
			button_SW1 = true; // call button press handler
			P2IFG &= ~BIT5; // reset the interrupt flag
		}
	}

	_BIC_SR_IRQ(LPM0_bits);
}

/*
 * waiting function to debounce the buttons
 */
void debounce_delay() {
	__delay_cycles(150);

//  OLD CODE
//	volatile unsigned int i; // volatile to prevent optimisation, which would mean get rid of this not very productive for-loop
//	for (i = 10000; i > 0; i--) {
//		;
//	}
//
//	/*
//	 * Value for i
//	 *
//	 * It seems the value for i can be a lot lower than initially given as an example (50.000).
//	 * It works fine with 5000 (one magnitude lower!) and okay, but not totally reliable with 500 (sometimes two toggles).
//	 * Reducing the value to 500 makes the buttons MUCH nicer to use, because the perceptible latency is completely removed.
//	 */
}


