#include <msp430.h>
#include <stdbool.h>


#include "sequence.h"

#include "lcd.h"
#include "input.h"
#include "tone.h"
#include "led_matrix.h"

enum Mode {
	edit,
	play,
};

Step sequence[16];

enum Mode mode;
unsigned int tempo; // unsigned, because the tempo should never be negative
unsigned int current_step;

bool outdated_display;

void button_SW4_pressed();
void button_SW3_pressed();
void button_SW2_pressed();
void button_SW1_pressed();
void encoder_left();
void encoder_right();
void potentiometer_turned();

void update_display();

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	lcd_init();
	input_init();
	ton_init();
	i2c_init();

	mode = edit; // the program starts in edit mode
	tempo = 220;  // the starting tempo is 120 bpm
	update_tempo(tempo);
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
		if (potentiometer_new || current_step != pot_value) {
			potentiometer_turned();
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
 * INPUT HANDLING
 */

void button_SW1_pressed() {
	if (mode == edit) {
		CCR1 = TAR + 100;
		update_tempo(tempo);
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
		update_tempo(tempo);
	}
}

// button to increase tempo
void button_SW4_pressed() {

	if (tempo < 500) {
		tempo += 10;
		outdated_display = true;
		update_tempo(tempo);
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
	if (mode == edit) {
		current_step = pot_value;
		led_on(current_step);
		ton(sequence[current_step].pitch, 10);
		outdated_display = true;
	}
}

/*
 * Play mode
 */

void play_next_step() {
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

// is called whenever the display needs to be updated, rewrites the whole display
void update_display() {
	// |#C3      120 bpm| as an example | D6      080 bpm|
	// |[O...]Bearbeiten| or:			|[OOO.] Abspielen|
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
    write_pitch(sequence[current_step].pitch);
    write_tone_length(sequence[current_step].tone_length);
    write_tempo(tempo);
}
