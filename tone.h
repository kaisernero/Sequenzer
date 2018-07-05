/*
 * piezoabgespeckt.h
 *
 *  Created on: 05.06.2018
 *      Author: jschmid6
 */

#ifndef TONE_H_
#define TONE_H_

#include <math.h>     // höhere Mathematik für die Berechnung der Frequenz

unsigned int m;	  // Deklaration der Zählvariable
unsigned int ccn; // Nummer, um die CCR2 nach jedem jeweiligen Interrupt inkrementert wird, um die nächste halbe Periodendauer festzulegen
unsigned int t;   // Limit für die halben  Perioden
unsigned int step_CC_number;  // Nummer, um die CCR1 nach jedem jeweiligen Interrupt inkrementiert wird, um die Dauer bis zum nächsten Schritt festzulegen

void ton_init();    // Initialisierung des Timers A für die Tonerzeugung
void ton(unsigned int pitch, unsigned long Dauer);  // Tonerzeugung
void update_tempo(unsigned int tempo);  // Berechnung der neuen step_CC_number

__interrupt void ton_umschalten();  // Interrupt von Timer A für Tonerzeugung und Schrittstart

#endif /* TONE_H_ */
