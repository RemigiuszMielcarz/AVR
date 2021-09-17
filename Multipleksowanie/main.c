/*
 * main.c
 *
 *  Created on: 18 kwi 2021
 *      Author: dell
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "MULTIPLEKS/multipleks.h"

int main(void)
{
	multipleks_init();
	//SEGMENTY_PORT &= ~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G|SEG_DP);
	//ANODY_PORT &= ~(ANODA_1|ANODA_2|ANODA_3|ANODA_4);

	sei(); // Zezwolenie na globalne przerwania

	cyfra[0] = 9;
	cyfra[1] = 8;
	cyfra[2] = 7;
	cyfra[3] = 6;
	_delay_ms(500);
	cyfra[0] = 10;
	cyfra[1] = 10;
	cyfra[2] = 10;
	cyfra[3] = 10;
	_delay_ms(500);

	uint16_t licznik = 5000;
	uint8_t z1, z2, z3, z4;

	while(1)
	{
		z1 = licznik/1000;
		if(z1) cyfra[0] = z1; else cyfra[0] = 10;
		z2 = (licznik-(z1*1000))/100;
		if(z2) cyfra[1] = z2; else cyfra[1] = (licznik>999)?0:10;
		z3 = (licznik-(z1*1000)-(z2*100))/10;
		if(z3) cyfra[2] = z3; else cyfra[2] = (licznik>99)?0:10;
		z4 = (licznik-(z1*1000)-(z2*100)-(z3*10));
		cyfra[3] = z4;
		_delay_ms(50);

		licznik--;
		if(!licznik) licznik = 5000;
	}
}
