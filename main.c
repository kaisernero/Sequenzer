// externe Bibliotheken für den MSP430 und den Datentyp bool
#include <msp430.h>
#include <stdbool.h>

// eigene Module
#include "sequence.h"
#include "tone.h"

// eigene Bibliotheken
#include "lcd.h"
#include "input.h"
#include "led_matrix.h"

// die zwei verschiedenen Modi, in denen der Sequenzer sein kann
enum Mode {
	edit,
	play,
};
enum Mode mode; // speichern des aktuellen Modus

// Flagge für Aktualisierung des Displays
bool outdated_display;

// Funktionen, die die Ereignisse nach Abfrage der Flaggen bedienen
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

    // Initialisierung der Module
	lcd_init();
	input_init();
	ton_init();
	i2c_init();


	// Initialisierung der Variablen mit Werten
	mode = edit; // the program starts in edit mode
	tempo = 220;  // the starting tempo is 120 bpm
	update_tempo(tempo);
	outdated_display = true;
	current_step = 0;

	// Startsequenz: Super Mario Melodie
	sequence[0].pitch = 56;
	sequence[0].tone_length = full;
	sequence[1].pitch = 56;
	sequence[1].tone_length = full;
	sequence[2].pitch = 56;
	sequence[2].tone_length = pause;
	sequence[3].pitch = 56;
	sequence[3].tone_length = full;
	sequence[4].pitch = 56;
	sequence[4].tone_length = pause;
	sequence[5].pitch = 52;
	sequence[5].tone_length = full;
	sequence[6].pitch = 56;
	sequence[6].tone_length = full;
	sequence[7].pitch = 56;
	sequence[7].tone_length = pause;
	sequence[8].pitch = 59;
	sequence[8].tone_length = full;
	sequence[9].pitch = 59;
	sequence[9].tone_length = pause;
	sequence[10].pitch = 59;
	sequence[10].tone_length = pause;
	sequence[11].pitch = 59;
	sequence[11].tone_length = pause;
	sequence[12].pitch = 47;
	sequence[12].tone_length = full;
	sequence[13].pitch = 47;
	sequence[13].tone_length = pause;
	sequence[14].pitch = 47;
	sequence[14].tone_length = pause;
	sequence[15].pitch = 47;
	sequence[15].tone_length = pause;

	int i;
	for(i=0; i<16; i++){
		sequence[i].pitch += 12;
	}

	// starten der Anzeigen: LED-Matrix und LC-Display
	led_on(current_step);
    update_display();

    // interrupts freigeben und damit Start des normalen Programmablaufs
	__enable_interrupt();

	// Schlafmodus für den Prozessor, bis ein interrupt auftritt und entsprechende Flaggen setzt
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
		// TODO: welcher low power mode?
	}
}

/*
 * INPUT HANDLING
 */
// Modus-Taster: wechselt zwischen den zwei Modi
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

// Tonlängen-Taster: erhöht die Tonlänge (bis zum Überlauf)
void button_SW2_pressed() {
	if (mode == edit) {
		if (sequence[current_step].tone_length < full)
			sequence[current_step].tone_length++;
		else
			sequence[current_step].tone_length = pause;

		outdated_display = true;
	}
}

// Tempo-Taster: Tempo verringern (um 10bpm)
void button_SW3_pressed() {
	if (tempo > 50) {
		tempo -= 10;
		outdated_display = true;
		update_tempo(tempo);
	}
}

// Tempo-Taster: Tempo erhöhen (um 10bpm)
void button_SW4_pressed() {
	if (tempo < 500) {
		tempo += 10;
		outdated_display = true;
		update_tempo(tempo);
	}
}

// Drehgeber wurde nach "unten" gedreht
void encoder_left() {
	// Tonhöhe nur im Bearbeiten-Modus verringern und wenn größer als die niedrigste Tonhöhe
	if (mode == edit && sequence[current_step].pitch > 36) {
		sequence[current_step].pitch--;
		outdated_display = true;
	}
}

// Drehgeber wurde nach "oben" gedreht
void encoder_right() {
	// Tonhöhe nur im Bearbeiten-Modus erhöhen und wenn niedriger als die höchste Tonhöhe
	if (mode == edit && sequence[current_step].pitch < 96) {
		sequence[current_step].pitch++;
		outdated_display = true;
	}
}

// Potentiometer wurde gedreht
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
// wird von tone.c aufgerufen, sobald der nächste Ton abgespielt werden soll
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

// wird aufgerufen, sobald die Anzeige des Display aktualisiert werden muss und beschreibt das komplette Display neu
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
