/*
 * piezoabgespeckt.h
 *
 *  Created on: 05.06.2018
 *      Author: jschmid6
 */

#ifndef PIEZOABGESPECKT_H_
#define PIEZOABGESPECKT_H_

#include <math.h>

unsigned int m=0;				//Deklaration der Zählvariable
unsigned int ccn;
unsigned int t;

//=====INITIALISIERUNG=====
void piezo_T_A_init (void){
	TACTL = TASSEL0 + TACLR + TAIE;     	// ACLK, clear TAR, interrupt enabled
	CCR2 = 0x0010;        			// Startwert CCR2
	P2DIR |=0x10;                  	// Portpin 2.4 Ausgang (Beeper)

	P2SEL |=0x10;                 	// alternative Peripherieeinheit (Timer A)

	TACTL |= MC1;

	CCTL2 = CCIE;
}
//=====


//=====TONERZEUGUNG=====
void ton(unsigned int pitch, unsigned long Dauer) {
	pitch = (int) (pow((double) 2, ((double) pitch - 57)/12) * 440);
	m = 0;
	ccn = 32768 / (pitch * 2);				//Berechnung des Übereinstimmungswertes für CCR2
	t = (Dauer * pitch) / 50; 			//Berechnung des Zählvariablenlimits
	CCR2 = TAR + ccn;      				// neuen Übereinstimmungswert berechnen

	CCTL2 = OUTMOD_4 + CCIE;
}
//=====

//=====INTERRUPTS=====
#pragma vector=TIMERA1_VECTOR				//Timer A verursacht Interrupt
__interrupt void ton_umschalten(void){
	switch( TAIV ){
	case 2:
		CCR1 += step_CC_number;
		play_next_step();
		_BIC_SR_IRQ(LPM0_bits);
		break;
	case 4:
		if(m<=t){
			CCR2 += ccn;      					// neuen Übereinstimmungswert eintragen
			m++;
		}
		else
			CCTL2 = CCIE;
		break;
	case 10:
		break;
	}
}

#endif /* PIEZOABGESPECKT_H_ */




///*
// * piezoabgespeckt.h
// *
// *  Created on: 05.06.2018
// *      Author: jschmid6
// */
//
//#ifndef PIEZOABGESPECKT_H_
//#define PIEZOABGESPECKT_H_
//
//#include <math.h>
//
//unsigned int m=0;				//Deklaration der Zählvariable
//unsigned int ccn;
//unsigned int t;
//
//void play_next_step();
//
////=====INITIALISIERUNG=====
//void piezo_T_A_init (void){
//	TACTL = TASSEL0 + TACLR + TAIE + MC1;     	// ACLK, clear TAR, interrupt enabled
//	CCTL2 = OUTMOD_4 + CCIE;     			// CCR2 toggle, interrupt enabled
//	CCR2 = 0x0010;        			// Startwert CCR2
//	P2DIR |= 0x10;                  	// Portpin 2.4 Ausgang (Beeper)
//	P2SEL = 0x00;
//	P2OUT &= ~0x10;
//}
//
////=====TONERZEUGUNG=====
////Ton: Bezeichnung, z.B. A4; Dauer: Dauer des Tons in Hundertstelsekunden

////=====
//
////=====INTERRUPTS=====
//#pragma vector=TIMERA1_VECTOR				//Timer A verursacht Interrupt
//__interrupt void ton_umschalten(void){
//	switch( TAIV ){
//	case 2:
////		CCR1 += step_CC_number;
////		play_next_step();
//		break;
//	case 4:
//		if(m <= t) {
//			CCR2 += ccn;      					// neuen Übereinstimmungswert eintragen
//			m++;
//		}
//		else {
//			P2SEL = 0x00;
//		}
//		break;
//	case 10:
//		break;
//	}
//}
//
//#endif /* PIEZOABGESPECKT_H_ */
