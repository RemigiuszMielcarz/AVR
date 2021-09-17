#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define INIT_LED0 DDRA |= (1<<PA0)
#define INIT_LED1 DDRA |= (1<<PA1)
#define INIT_LED2 DDRA |= (1<<PA2)
#define INIT_LED3 DDRA |= (1<<PA3)
#define INIT_LED4 DDRA |= (1<<PA4)
#define INIT_LED5 DDRA |= (1<<PA5)
#define INIT_LED6 DDRA |= (1<<PA6)


#define OFF_SEG_A  PORTA |= (1<<PA0)
#define ON_SEG_A PORTA &= ~(1<<PA0)
#define OFF_SEG_B  PORTA |= (1<<PA1)
#define ON_SEG_B PORTA &= ~(1<<PA1)
#define OFF_SEG_C  PORTA |= (1<<PA2)
#define ON_SEG_C PORTA &= ~(1<<PA2)
#define OFF_SEG_D  PORTA |= (1<<PA3)
#define ON_SEG_D PORTA &= ~(1<<PA3)
#define OFF_SEG_E  PORTA |= (1<<PA4)
#define ON_SEG_E PORTA &= ~(1<<PA4)
#define OFF_SEG_F  PORTA |= (1<<PA5)
#define ON_SEG_F PORTA &= ~(1<<PA5)
#define OFF_SEG_G  PORTA |= (1<<PA6)
#define ON_SEG_G PORTA &= ~(1<<PA6)


// Definicje portu i rejestru kierunkowego dla anod wyœwietlacza
#define ANODY_PORT			PORTC
#define ANODY_KIERUNEK		DDRC

// Definicje bitow dla poszczegolnych anod
#define ANODA_1				(1<<PC7)
#define ANODA_2				(1<<PC6)
#define ANODA_3				(1<<PC5)
#define ANODA_4				(1<<PC4)
#define MASKA_ANODY (ANODA_1 | ANODA_2 | ANODA_3 | ANODA_4) //11110000

#define ON_DIS_1 PORTC &= ~(1<<PC7)
#define ON_DIS_2 PORTC &= ~(1<<PC6)
#define ON_DIS_3 PORTC &= ~(1<<PC5)
#define ON_DIS_4 PORTC &= ~(1<<PC4)

#define OFF_DIS_1 PORTC |= (1<<PC7)
#define OFF_DIS_2 PORTC |= (1<<PC6)
#define OFF_DIS_3 PORTC |= (1<<PC5)
#define OFF_DIS_4 PORTC |= (1<<PC4)

void wylaczwszystkie(void)
{
	OFF_SEG_A;
	OFF_SEG_B;
	OFF_SEG_C;
	OFF_SEG_D;
	OFF_SEG_E;
	OFF_SEG_F;
	OFF_SEG_G;
}

void init(void)
{
	INIT_LED0;
	INIT_LED1;
	INIT_LED2;
	INIT_LED3;
	INIT_LED4;
	INIT_LED5;
	INIT_LED6;
	wylaczwszystkie();

	ANODY_KIERUNEK |= ANODA_1 | ANODA_2 | ANODA_3 | ANODA_4;	// Ustawienie anod na wyjscia

	ANODY_PORT |= ANODA_1 | ANODA_2 | ANODA_3 | ANODA_4;		// Wygaszenie wszystkich wyswietlaczy - stan wysoki 11110000
}

void display_number(int no)
{
	switch(no)
	{
		case 0:
			wylaczwszystkie();
			ON_SEG_A;
			ON_SEG_B;
			ON_SEG_C;
			ON_SEG_D;
			ON_SEG_E;
			ON_SEG_F;
			break;
		case 1:
			wylaczwszystkie();
			ON_SEG_B;
			ON_SEG_C;
			break;
		case 2:
			wylaczwszystkie();
			ON_SEG_A;
			ON_SEG_B;
			ON_SEG_G;
			ON_SEG_E;
			ON_SEG_D;
			break;
		case 3:
			wylaczwszystkie();
			ON_SEG_A;
			ON_SEG_B;
			ON_SEG_G;
			ON_SEG_C;
			ON_SEG_D;
			break;
		case 4:
			wylaczwszystkie();
			ON_SEG_F;
			ON_SEG_G;
			ON_SEG_C;
			ON_SEG_B;
			break;
		case 5:
			wylaczwszystkie();
			ON_SEG_A;
			ON_SEG_F;
			ON_SEG_G;
			ON_SEG_C;
			ON_SEG_D;
			break;
		case 6:
			wylaczwszystkie();
			ON_SEG_A;
			ON_SEG_F;
			ON_SEG_G;
			ON_SEG_E;
			ON_SEG_C;
			ON_SEG_D;
			break;
		case 7:
			wylaczwszystkie();
			ON_SEG_A;
			ON_SEG_C;
			ON_SEG_B;
			break;
		case 8:
			wylaczwszystkie();
			ON_SEG_A;
			ON_SEG_F;
			ON_SEG_G;
			ON_SEG_C;
			ON_SEG_D;
			ON_SEG_B;
			ON_SEG_E;
			break;
		case 9:
			wylaczwszystkie();
			ON_SEG_A;
			ON_SEG_F;
			ON_SEG_G;
			ON_SEG_C;
			ON_SEG_B;
			ON_SEG_D;
			break;
	}
}

void display_7seg(int no1)
{
	switch(no1)
	{
		case 1:
			ON_DIS_1;
			OFF_DIS_2;
			OFF_DIS_3;
			OFF_DIS_4;
			break;
		case 2:
			ON_DIS_2;
			OFF_DIS_1;
			OFF_DIS_3;
			OFF_DIS_4;
			break;
		case 3:
			ON_DIS_3;
			OFF_DIS_1;
			OFF_DIS_2;
			OFF_DIS_4;
			break;
		case 4:
			ON_DIS_4;
			OFF_DIS_1;
			OFF_DIS_2;
			OFF_DIS_3;
			break;
	}
}

int main(void)
{
	init();

	int pomiar, z1, z2, z3, z4;
	int cyfra[4];

    ADMUX |= ((1 <<  REFS0) | (1 << MUX0) | (1 << MUX1) | (1 << MUX2)); //wybor zrodla napiecia odniesienia (AVCC (+5V) s.214) i pinu pomiarowego ADC (ADC7, bo na pinie PA7 (s.215))

    ADCSRA |= ((1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2)); //uruchomienie przetwornika (s.216) i ustawienie czestotliwosci jego pracy (preskaler 128 (s.217))

	//Alternatywne sprawdzenie
	/*
	for(int i=0; i<4; i++)
	{
		_delay_ms(1);
		ANODY_PORT = (ANODY_PORT | MASKA_ANODY); //Wy³¹czenie wyswietlaczy
		display_number(i+1);
		ANODY_PORT = ( ANODY_PORT & ~MASKA_ANODY ) | (~(1<<i) & MASKA_ANODY); // Cykliczne przelaczanie wyswietlaczy
		_delay_ms(1);
	}
	*/

	//Sprawdzenie czy dzia³a wyœwietlanie
	for(int i=0; i<4; i++)
	{
		_delay_ms(500);
		display_7seg(i+1);
		display_number(i+1);
		_delay_ms(500);
	}

    while(1)
	{
		ADCSRA |= (1 << ADSC); // uruchamia konwersje (s.216)

		while(ADCSRA & (1 << ADSC)); // oczekiwanie na zakonczenie konwersji

		pomiar = ADC; //odczyt pomiaru z przetwornika

		z1 = pomiar/1000;
		if(z1<1) cyfra[0] = 0; else cyfra[0]=1;
		z2 = (pomiar-(z1*1000))/100;
		cyfra[1] = z2;
		z3 = (pomiar-(z1*1000)-(z2*100))/10;
		cyfra[2] = z3;
		z4 = (pomiar-(z1*1000)-(z2*100)-(z3*10));
		cyfra[3] = z4;


		for(int i=0; i<4; i++)
		{
			_delay_ms(1);
			display_number(cyfra[i]);
			display_7seg(i+1);
			if(cyfra[0]==0) OFF_DIS_1;
			if(cyfra[0]==0 && cyfra [1]==0)
			{
			OFF_DIS_1;
			OFF_DIS_2;
			}
			if(cyfra[0]==0 && cyfra[1]==0 && cyfra[2]==0)
			{
			OFF_DIS_1;
			OFF_DIS_2;
			OFF_DIS_3;
			}
			_delay_ms(1);
		}
	}
	return 0;
}
