/*
 * multipleks.c
 *
 *  Created on: 18 kwi 2021
 *      Author: dell
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "multipleks.h"

// Definicja tablicy zawierajacej definicje bitowe cyfr na wyswietlacz 7-seg
const uint8_t cyfry[] PROGMEM = {
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F),			//0
		~(SEG_B|SEG_C),									//1
		~(SEG_A|SEG_B|SEG_D|SEG_E|SEG_G),				//2
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_G),				//3
		~(SEG_B|SEG_C|SEG_F|SEG_G),						//4
		~(SEG_A|SEG_C|SEG_D|SEG_F|SEG_G),				//5
		~(SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G),			//6
		~(SEG_A|SEG_B|SEG_C|SEG_F),						//7
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G),	//8
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G),			//9
		0xFF											//10
};

uint8_t cyfra[4]; // Zmienna tablicowa (4 wyœwietlacze)

void multipleks_init(void)
{
	SEGMENTY_KIERUNEK = 0xff; // Ustawienie rejestru kierunkowego portu segmentow na wyjscie
	SEGMENTY_PORT = 0xff;	  // Wygaszenie wszystkich katod - stan wysoki

	ANODY_KIERUNEK |= ANODA_1 | ANODA_2 | ANODA_3 | ANODA_4;	// Ustawienie anod na wyjscia
	ANODY_PORT |= ANODA_1 | ANODA_2 | ANODA_3 | ANODA_4;		// Wygaszenie wszystkich wyswietlaczy - stan wysoki

	// Ustawienia TIMER0
	TCCR0 |= (1<<WGM01); // Tryb CTC
	TCCR0 |= (1<<CS02);  // Preskaler = 256
	OCR0 = 215;			 // Dodatkowy podzial przez 215
	TIMSK |= (1<<OCIE0); // Zezwolenie na przerwanie CompareMatch
}

ISR(TIMER0_COMP_vect)
{
	static uint8_t licznik;
	ANODY_PORT = (ANODY_PORT | MASKA_ANODY);

	SEGMENTY_PORT = pgm_read_byte(&cyfry[cyfra[licznik]]);

	ANODY_PORT = ( ANODY_PORT & ~MASKA_ANODY ) | (~(1<<licznik) & MASKA_ANODY); // Cykliczne przelaczanie wyswietlaczy

	licznik++;
	if(licznik > 3) licznik =0;
}
