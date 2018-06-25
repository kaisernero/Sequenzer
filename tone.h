/*
 * piezoabgespeckt.h
 *
 *  Created on: 05.06.2018
 *      Author: jschmid6
 */

#ifndef PIEZOABGESPECKT_H_
#define PIEZOABGESPECKT_H_

#include <math.h>

unsigned int m;				//Deklaration der Zählvariable
unsigned int ccn;
unsigned int t;
unsigned int step_CC_number;

void ton_init();
void ton(unsigned int pitch, unsigned long Dauer);
void update_tempo(unsigned int tempo);

__interrupt void ton_umschalten();

#endif /* PIEZOABGESPECKT_H_ */
