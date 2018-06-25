#include <msp430.h>
#include <stdbool.h>

unsigned int step_CC_number;

#include "lcd.h"
#include "input.h"
#include "piezoabgespeckt.h"
#include "led_matrix.h"

const unsigned char NOTE_NAMES[12][3] = {" C", "#C", " D", "#D", " E", " F", "#F", " G", "#G", " A", "#A", " B"};

enum Mode {
	edit,
	play,
};

enum Tone_length {
	pause,
	quarter,
	half,
	three_quarters,
	full,
};

typedef struct Step {
	unsigned int pitch;
	enum Tone_length tone_length;
} Step;

Step sequence[16];

enum Mode mode;
unsigned int tempo; // unsigned, because the tempo should never be negative
bool outdated_display;
unsigned int current_step;

void button_SW4_pressed();
void button_SW3_pressed();
void button_SW2_pressed();
void button_SW1_pressed();
void encoder_left();
void encoder_right();

void calculate_CC_number();
void write_pitch(unsigned int pitch);
void write_tone_length(enum Tone_length tone_length);
void write_tempo(unsigned int tempo);
void update_display();

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	lcd_init();
	input_init();
	piezo_T_A_init();
	i2c_init();

	mode = edit; // the program starts in edit mode
	tempo = 220;  // the starting tempo is 120 bpm
	calculate_CC_number();
	CCTL1 = CCIE;

	outdated_display = true;

	// TODO: correct initialisation
	unsigned int i;
	for (i = 0; i < 16; i++) {
		sequence[i].pitch = 36 + 3*i;
		sequence[i].tone_length = full;
	}

	current_step = 0;
	led_on(current_step);

    update_display(); // to show the current status on the display from the start

    // start normal function of the alarm clock by first enabling the interrupts and then going to sleep
	__enable_interrupt();


	// let the msp go to sleep. only react to interrupts
	while (1) {
		if (encoder_l) {
			encoder_left();
			encoder_l = false;
		}
		else if (encoder_r) {
			encoder_right();
			encoder_r = false;
		}
		else if (button_SW4) {
			button_SW4_pressed();
			button_SW4 = false;
		}
		else if (button_SW3) {
			button_SW3_pressed();
			button_SW3 = false;
		}
		else if (button_SW2) {
			button_SW2_pressed();
			button_SW2 = false;
		}
		else if (button_SW1) {
			button_SW1_pressed();
			button_SW1 = false;
		}
		if (potentiometer_new || current_step != pot_value && mode == edit) {
			current_step = pot_value;
			led_on(current_step);
			ton(sequence[current_step].pitch, 10);
			outdated_display = true;
			potentiometer_new = false;
		}

		if (outdated_display) {
			update_display();
		    outdated_display = false;
		}
		if ((encoder_l + encoder_r + button_SW4 + button_SW3 + button_SW2 + button_SW1 + outdated_display) == 0)
			LPM0; // Low power mode 0
		// to wake up: LPM0_EXIT;
	}
}

/*
 * INPUTS
 */

void button_SW1_pressed() {
	if (mode == edit) {
		CCR1 = TAR + 100;
		calculate_CC_number();
		current_step = 0;
		led_on(current_step);
		mode = play;
	}
	else {
		mode = edit;
	}

	outdated_display = true;
}

void button_SW2_pressed() {
	if (mode == edit) {
		if (sequence[current_step].tone_length < full)
			sequence[current_step].tone_length++;
		else
			sequence[current_step].tone_length = pause;

		outdated_display = true;
	}
}

// button to decrease tempo
void button_SW3_pressed() {
	if (tempo > 50) {
		tempo -= 10;
		outdated_display = true;
		calculate_CC_number();
	}
}

// button to increase tempo
void button_SW4_pressed() {

	if (tempo < 500) {
		tempo += 10;
		outdated_display = true;
		calculate_CC_number();
	}
}

// turning the encoder "down"
void encoder_left() {
	// only decrease pitch when in edit mode and higher than the lowest pitch
	if (mode == edit && sequence[current_step].pitch > 36) {
		sequence[current_step].pitch--;
		outdated_display = true;
	}
}

// turning the encoder "up"
void encoder_right() {
	// only increase pitch when in edit mode and lower than the highest pitch
	if (mode == edit && sequence[current_step].pitch < 96) {
		sequence[current_step].pitch++;
		outdated_display = true;
	}
}

void potentiometer_turned() {
//	if (mode == edit) {
//		unsigned int i;
//		i = potentiometer_value >> 6; // 10 bit (0 to 1023) ADC to 4 bit (0 to 15) for the current_step
//		if (current_step != i) {
//			current_step = i;
//			//ton(sequence[current_step].pitch, 100);
//			outdated_display = true;
//		}
//	}
}

/*
 * Play mode
 */

// recalculates the CC-number to the current tempo in bpm
void calculate_CC_number() {
	step_CC_number = (int) (32768 * 60 / tempo);
}

void play_next_step() {
	unsigned int current_tone_length = 60 / tempo;
	if (mode == play) {
		switch (sequence[current_step].tone_length) {
		case quarter:
			ton(sequence[current_step].pitch, (int) 1500 / tempo);
			break;
		case half:
			ton(sequence[current_step].pitch, (int) 3000 / tempo);
			break;
		case three_quarters:
			ton(sequence[current_step].pitch, (int) 4500 / tempo);
			break;
		case full:
			ton(sequence[current_step].pitch, (int) 6000 / tempo);
			break;
		}
		led_on(current_step);
		current_step++;
		if (current_step == 16)
			current_step = 0;
		outdated_display = true;
	}
}

/*
 * LCD
 */

void write_pitch(unsigned int pitch) {
    lcd_set_cursor(0, 0);
	// write the note name with the corresponding accidental (or not)
	lcd_write_string(NOTE_NAMES[(int) pitch % 12]);
	// write the octave number last
	lcd_write_int((int) pitch / 12, 1);
}

void write_tone_length(enum Tone_length tone_length) {
	lcd_set_cursor(0, 1);
	lcd_write('[');
	// TODO: different characters
	switch (tone_length) {
	case pause:
		lcd_write_string("....");
		break;
	case quarter:
		lcd_write_string("O...");
		break;
	case half:
		lcd_write_string("OO..");
		break;
	case three_quarters:
		lcd_write_string("OOO.");
		break;
	case full:
		lcd_write_string("OOOO");
		break;
	}
	lcd_write(']');
}

void write_tempo(unsigned int tempo) {
	lcd_set_cursor(9, 0);
	// TODO: leading 0 removal in lcd_write_int
	lcd_write_int(tempo, 3);
	lcd_write_string(" bpm");
}

void update_display() {
	switch (mode) {
	case edit:
		lcd_set_cursor(6, 1);
		lcd_write_string("Bearbeiten");
		break;
	case play:
		lcd_set_cursor(6, 1);
		lcd_write_string(" Abspielen");
		break;
	}

	// TODO: replace with showing the step with the LED-matrix
	lcd_set_cursor(5, 0);
	lcd_write_int(current_step+1, 2);

    write_pitch(sequence[current_step].pitch);
    write_tone_length(sequence[current_step].tone_length);
    write_tempo(tempo);
}
