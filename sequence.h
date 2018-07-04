/*
 * sequence.h
 *
 * Definitionen der Datenstrukturen und Variablen für die Sequenz
 */

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

enum Tone_length {
	pause,
	quarter,
	half,
	three_quarters,
	full,
};

struct Step {
	unsigned int pitch;
	enum Tone_length tone_length;
};

struct Step sequence[16];

unsigned int tempo;
unsigned int current_step; // aktuelle Position innerhalb des Sequenz

#endif /* SEQUENCE_H_ */
