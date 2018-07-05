/*
 * tone.c
 *
 *  Created on: 25.06.2018
 *      Author: draab
 */
#include <msp430.h>
#include "tone.h"

void ton_init() {
	TACTL = TASSEL0 + TACLR + TAIE;     	// ACLK, clear TAR, interrupt enabled
	CCR2 = 0x0010;        			// Startwert CCR2

	P2DIR |=0x10;                  	// Portpin 2.4 Ausgang (Beeper)
	P2SEL |=0x10;                 	// alternative Peripherieeinheit (Timer A)

	TACTL |= MC1; 			// start Timer A
	CCTL2 = CCIE;			// CCR2 interrupt enable
}

void ton(unsigned int pitch, unsigned long Dauer) {
	__enable_interrupt();		// Interrupt darf unterbrochen werden, weil Berechnung von pitch lange dauert
	pitch = (int) (pow((double) 2, ((double) pitch - 57)/12) * 440);	// Berechnung der Frequenz aus der Tonhöhe
	m = 0;				// m muss wieder von vorne zählen
	ccn = 32768 / (pitch * 2);				//Berechnung des Übereinstimmungswertes für CCR2
	t = (Dauer * pitch) / 50; 			//Berechnung des Zählvariablenlimits
	CCR2 = TAR + ccn;      				// neuen Übereinstimmungswert berechnen

	CCTL2 = OUTMOD_4 + CCIE;	// CCR2 modus: toggle, interrupt enable
	CCTL1 = CCIE;			// CCR1 interrupt enable
}

void update_tempo(unsigned int tempo) {
	// recalculates the CC-number(number to add to the capture/compare register) to the current tempo in bpm
	step_CC_number = (int) (32768 * 60 / tempo);
}

#pragma vector=TIMERA1_VECTOR				//Timer A verursacht Interrupt
__interrupt void ton_umschalten(void){
	switch(TAIV) {
	case 2:
		CCR1 += step_CC_number;			// zeit bis zum nächsten schritt
		play_next_step(); // definiert in main.c, startet den nächsten Schritt und spielt dessen Ton ab
		_BIC_SR_IRQ(LPM0_bits);			// Flaggen abfragen und abarbeiten
		break;
	case 4:
		if(m<=t){				// wenn Limit nicht erreicht ist (Ton noch nicht zu ende)
			CCR2 += ccn;      		// neuen Übereinstimmungswert eintragen
			m++;				// halbe Periode vergangen
		}
		else
			CCTL2 = CCIE;			// OUTMOD_4 ausschalten (implizit, da im Register CCTL2 nur das Bit CCIE gesetzt wird)
		break;
	case 10:
		break;
	}
}
