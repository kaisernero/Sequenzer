/*
 * Konstanten.h
 *
 *  Created on: 14.05.2018
 *      Author: jschmid6
 * Hier sind alle für die Ton- und Melodieerzeugung wichtigen Konstanten hinterlegt
 */

#ifndef KONSTANTEN_H_
#define KONSTANTEN_H_

#define Pause 5
// Oktave 3 ------------------------ Bezeichnung des Tones und zugehörige Frequenz in Hertz
#define C3 130.813
#define Cis3 138.591
#define D3 146.832
#define Dis3 155.563
#define E3 164.814
#define F3 174.614
#define Fis3 184.997
#define G3 195.998
#define Gis3 207.652
#define A3 220.000
#define Ais3 233.082
#define B3 246.942
// Oktave 4 ------------------------
#define C4 261.626
#define Cis4 277.183
#define D4 293.665
#define Dis4 311.127
#define E4 329.628
#define F4 349.228
#define Fis4 369.994
#define G4 391.995
#define Gis4 415.305
#define A4 440.000
#define Ais4 466.164
#define B4 493.883
//Oktave 5 -------------------------
#define C5 523.251
#define Cis5 554.365
#define D5 587.330
#define Dis5 622.254
#define E5 659.255
#define F5 698.456
#define Fis5 739.989
#define G5 783.991
#define Gis5 830.609
#define A5 880.000
#define Ais5 932.328
#define B5 987.767
//Oktave 6 -------------------------
#define C6 1046.50
#define Cis6 1108.73
#define D6 1174.66
#define Dis6 1244.51
#define E6 1318.51
#define F6 1396.91
#define Fis6 1479.98
#define G6 1567.98
#define Gis6 1661.22
#define A6 1760.00
#define Ais6 1864.66
#define B6 1975.53
//=====

//=====MELODIEN=====
const int melodie1 [19] = {9, A5, F5, G5, C5, Pause,  C5, G5, A5, F5, //glockenturm
							  30, 30, 30, 60, 30, 	  30, 30, 30, 60};

const int melodie2 [51] = {25, B4, A4, Gis4, A4, C5, Pause, D5, C5, B4, C5, E5, Pause, F5, E5, Dis5, E5, B5, A5, Gis5, A5, B5, A5, Gis5, A5, C6, //rondo alla turca
							   10, 10, 10,   10, 30, 10,    10, 10, 10, 10, 30, 10,    10, 10, 10,   10, 10, 10, 10,   10, 10, 10, 10,   10, 40};

const int melodie3 [11] = {5, A3, Pause, A3, Pause, G3, 40, 40, 40, 40, 40}; //testmelodie für pause
//=====

#endif /* KONSTANTEN_H_ */
