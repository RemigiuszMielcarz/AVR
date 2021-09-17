/*
 * main.c
 *
 *  Created on: 21 mar 2021
 *      Author: dell
 */

#include <avr/io.h>
#include <util/delay.h>

//LED podlaczony katod¹ do pinu, anodami do VCC
#define LED_PIN (1<<PC0) //definicja pinu, do ktorego podlaczona jest dioda
#define LED_ON PORTC &= ~LED_PIN // makrodefinicja - zalaczenie diody
#define LED_OFF PORTC |= LED_PIN // wyl
#define LED_TOG PORTC ^= LED_PIN // zmiana stanu diody

// **** pierwsza wersja
int main (void)
{
	// inicjalizacja
	DDRC |= LED_PIN; //kierunek pinu PC0 - wyjsciowy

	while(1)
	{
		LED_ON;
		_delay_ms(1000);
		LED_OFF;
		_delay_ms(1000);
	}
}
