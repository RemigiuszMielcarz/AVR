/*
 * main.c
 *
 *  Created on: 11 sie 2021
 *      Author: dell
 */


#include <avr/io.h>
#include <util/delay.h>

//Jezeli bity COMxx beda ustawione tak:
//TCCR0 |= (1<<COM01) | (1<<COM00); //clear OC0 at TOP
//to dioda LED bedzie sie rozjasniac w pierwszej petli wraz ze wzrostem wartosci w OCR0 i
//przygasac gdy wartosc w OCR0 bedzie malec
//przeciwnie bedzie gdy bedzie set OC0 at TOP

int main(void)
{
	DDRB |= (1<<PB3); //kierunek pinu PB3 - WYJŒCIE
	//ustawienia TIMER0 w tryb PWM
	TCCR0 |= (1<<WGM01) | (1<<WGM00); //tryb Fast PWM
	TCCR0 |= (1<<COM01) | (1<<COM00); //clear OC0 at TOP
	TCCR0 |= (1<<CS00);				  //preskaler = 1
	//Fpwm=F_CPU/prescaler/256
	//Fpwm=11059200/1/256=43,200k kHz
	OCR0 = 5; 	// minimalne wype³nienie i jasnosc diody LED
	OCR0 = 127; //   srednie wypelnienie i jasnosc diody LED
	OCR0 = 255; //maksymalne wypelnienie i jasnosc diody LED

	uint8_t i;
	while(1)
	{
		for(i=0;i<255;i++) //clear OC0 at top i mamy rozjasnianie
		{
			OCR0=i; //rozjasnienie lub sciemnienie w zaleznosci od COMxx
			_delay_ms(5);
		}
			for(i=255;i;i--)
			{
				OCR0=i;
				_delay_ms(5);
			}
		OCR0=5;
		_delay_ms(1000);
	}



}

