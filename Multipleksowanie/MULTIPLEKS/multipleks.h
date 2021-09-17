/*
 * multipleks.h
 *
 *  Created on: 18 kwi 2021
 *      Author: dell
 */

#ifndef MULTIPLEKS_MULTIPLEKS_H_
#define MULTIPLEKS_MULTIPLEKS_H_

// Definicje portu i rejestru kierunkowego dla segmentów wyœwietlacza
#define SEGMENTY_PORT		PORTA
#define SEGMENTY_KIERUNEK	DDRA

// Definicje portu i rejestru kierunkowego dla anod wyœwietlacza
#define ANODY_PORT			PORTB
#define ANODY_KIERUNEK		DDRB

// Definicje bitow dla poszczegolnych anod
#define ANODA_1				(1<<PB0)
#define ANODA_2				(1<<PB1)
#define ANODA_3				(1<<PB2)
#define ANODA_4				(1<<PB3)
#define MASKA_ANODY (ANODA_1 | ANODA_2 | ANODA_3 | ANODA_4)
// Definicje bitow dla poszczegolnych segmentow
#define SEG_A				(1<<0)
#define SEG_B				(1<<1)
#define SEG_C				(1<<2)
#define SEG_D				(1<<3)
#define SEG_E				(1<<4)
#define SEG_F				(1<<5)
#define SEG_G				(1<<6)
#define SEG_DP				(1<<7)

extern uint8_t cyfra[4];

void multipleks_init(void);


#endif /* MULTIPLEKS_MULTIPLEKS_H_ */
