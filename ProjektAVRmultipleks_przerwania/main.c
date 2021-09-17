#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// Definicje portu i rejestru kierunkowego dla segmentów wyœwietlacza
#define SEGMENTY_PORT		PORTA
#define SEGMENTY_KIERUNEK	DDRA

// Definicje portu i rejestru kierunkowego dla anod wyœwietlacza
#define ANODY_PORT			PORTC
#define ANODY_KIERUNEK		DDRC

// Definicje bitow dla poszczegolnych anod
#define ANODA_1				(1<<PC0)
#define ANODA_2				(1<<PC1)
#define ANODA_3				(1<<PC2)
#define ANODA_4				(1<<PC3)
#define MASKA_ANODY (ANODA_1 | ANODA_2 | ANODA_3 | ANODA_4) 	//0000 1111
// Definicje bitow dla poszczegolnych segmentow
#define SEG_A				(1<<0)
#define SEG_B				(1<<1)
#define SEG_C				(1<<2)
#define SEG_D				(1<<3)
#define SEG_E				(1<<4)
#define SEG_F				(1<<5)
#define SEG_G				(1<<6)

volatile uint8_t cyfra[4];

const uint8_t cyfry[15] = {
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F),			//0		~0011 1111 	czyli 1100 0000	pod³¹czone anodami do masy
		~(SEG_B|SEG_C),						//1		~0000 0110	czyli 1111 1001
		~(SEG_A|SEG_B|SEG_D|SEG_E|SEG_G),			//2		~0101 1011	czyli 1010 0100
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_G),			//3
		~(SEG_B|SEG_C|SEG_F|SEG_G),				//4
		~(SEG_A|SEG_C|SEG_D|SEG_F|SEG_G),			//5
		~(SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G),			//6
		~(SEG_A|SEG_B|SEG_C|SEG_F),				//7
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G),		//8
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G),			//9
};

void init(void) //inicjalizacja timera do obs³ugi wyœwietlaczy 7-segmentowych
{
	SEGMENTY_KIERUNEK |= SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G; // (DDRA = x111 1111) Ustawienie rejestru kierunkowego portu segmentow na wyjscie
	SEGMENTY_PORT |= SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;	  	// (PORTA = x111 1111) Wygaszenie wszystkich katod - stan wysoki

	ANODY_KIERUNEK |= ANODA_1 | ANODA_2 | ANODA_3 | ANODA_4;	// (DDRC = xxxx 1111) Ustawienie anod na wyjscia
	ANODY_PORT |= ANODA_1 | ANODA_2 | ANODA_3 | ANODA_4;		// (PORTC = xxxx 1111) Wygaszenie wszystkich wyswietlaczy - stan wysoki

	// Ustawienia TIMER0
	TCCR0 |= (1<<WGM01); // Tryb CTC
	TCCR0 |= (1<<CS02) | (1<<CS00); // Preskaler = 1024
	OCR0 = 53; // Dodatkowy podzial przez 53	(11059200 Hz / 1024 / 200Hz[oczekiwana wartoœc])-1 = 53
	TIMSK |= (1<<OCIE0); // Zezwolenie na przerwanie CompareMatch
}

ISR(TIMER0_COMP_vect)
{
	static uint8_t licznik = 1;
	ANODY_PORT = ~licznik;	//PORTC =

	if(licznik==1) 		SEGMENTY_PORT = (cyfry[cyfra[0]]);
	else if(licznik==2) SEGMENTY_PORT = (cyfry[cyfra[1]]);
	else if(licznik==4) SEGMENTY_PORT = (cyfry[cyfra[2]]);
	else if(licznik==8) SEGMENTY_PORT = (cyfry[cyfra[3]]);

	licznik <<= 1;
	if(licznik>8) licznik = 1;
}

int main(void)
{
	init();

	int pomiar;
	uint8_t z1, z2, z3, z4;

    ADMUX |= ((1 <<  REFS0) | (1 << MUX0) | (1 << MUX1) | (1 << MUX2)); //wybor zrodla napiecia odniesienia (AVCC (+5V) s.214) i pinu pomiarowego ADC (ADC7, bo na pinie PA7 (s.215))

    ADCSRA |= ((1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2)); //uruchomienie przetwornika (s.216) i ustawienie czestotliwosci jego pracy (preskaler 128 (s.217))

	sei(); // Zezwolenie na globalne przerwania

	cyfra[0] = 0;
	cyfra[1] = 8;
	cyfra[2] = 8;
	cyfra[3] = 0;
	_delay_ms(2500);
	cyfra[0] = 3;
	cyfra[1] = 5;
	cyfra[2] = 9;
	cyfra[3] = 2;
	_delay_ms(2500);

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
		_delay_ms(50);
	}
	return 0;
}
