/*
 * sequence.h
 *
 * contains the definitions of the data types needed for a sequence: struct step and as a part of step: enum Tone_length
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

typedef struct Step {
	unsigned int pitch;
	enum Tone_length tone_length;
} Step;

#endif /* SEQUENCE_H_ */
