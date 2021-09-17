/*
 * main.c
 *
 *  Created on: 21 mar 2021
 *      Author: dell
 */

#include <avr/io.h>
#include <util/delay.h>

//LED (podlaczony do VCC)
#define LED_PIN (1<<PC0) // definicja pinu, do ktorego podlaczona jest dioda
#define LED_TOG PORTC ^= LED_PIN // zmiana stanu diody
//Klawisz (podlaczony do GND)
#define KEY_PIN (1<<PC1) // definicja pinu, do ktorego podlaczony jest klawisz
#define KEY_DOWN !(PINC & KEY_PIN) // sprawdza czy jest stan niski

// **** pierwsza wersja
uint8_t klawisz_wcisniety(void); //deklaracja funkcji
int main (void)
{
	// inicjalizacja
	DDRC |= LED_PIN; //kierunek pinu PC0 - wyjsciowy
	PORTC |= LED_PIN; //wylaczenie diody LED
	DDRC &= ~KEY_PIN; //kierunek pinu PC6 - wejsciowy
	PORTC |= KEY_PIN; //podciagniecie pinu do VCC (wewnetrzny rezystor)

	while(1)
	{
		/*
		if( KEY_DOWN ) //klawisz wciœniêty?
		{
			_delay_ms(80);
			if( KEY_DOWN ) // klawisz nadal wciœniêty?
			*/
				if( klawisz_wcisniety() )
					{
						LED_TOG;
						_delay_ms(200);
					}

	}
}

// definicja funkcji
// funkcja nie posiada argumentów, zwraca wynik 0 lub 1

uint8_t klawisz_wcisniety(void)
{
	if( KEY_DOWN ) //klawisz wciœniêty?
	{
		_delay_ms(80); // czas drgañ styków
		if( KEY_DOWN ) // klawisz nadal wciœniêty?
			return 1;
	}
	return 0;
}
