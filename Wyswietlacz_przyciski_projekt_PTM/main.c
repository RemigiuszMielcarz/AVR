#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// Definijcje inicjalizacyjne dla diody LED
#define INIT_LED  DDRB  |= (1 << PB7)
#define ON_LED	  PORTB |= (1 << PB7)
#define OFF_LED	  PORTB &= ~(1 << PB7)

// Definicje inicjalizacyjne dla przycisków
#define INIT_SWSTART   DDRD &= ~(1<<PD2) //START
#define INIT_SWSTOP    DDRD &= ~(1<<PD3) //STOP
#define INIT_SWRESTART DDRB &= ~(1<<PB2) //RESTART

#define SW_CONFIG_PULLUP_START   PORTD |= (1 << PD2)
#define SW_CONFIG_PULLUP_STOP    PORTD |= (1 << PD3)
#define SW_CONFIG_PULLUP_RESTART PORTB |= (1 << PB2)

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
#define MASKA_ANODY (ANODA_1 | ANODA_2 | ANODA_3) 	//0000 0111

// Definicje bitow dla poszczegolnych segmentow
#define SEG_A				(1<<0)
#define SEG_B				(1<<1)
#define SEG_C				(1<<2)
#define SEG_D				(1<<3)
#define SEG_E				(1<<4)
#define SEG_F				(1<<5)
#define SEG_G				(1<<6)

// Zmienne
volatile uint8_t cyfra[4];
volatile int odliczanie      = 0;
volatile int wartosc         = 0;
volatile int pomiar          = 1; // Zezwól na pocz¹tku programu na odczytywanie z przetwornika
volatile uint8_t licznik_ovf = 0;

const uint8_t cyfry[15] = {
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F),			//0		~0011 1111 	czyli 1100 0000	pod³¹czone anodami do masy
		~(SEG_B|SEG_C),						            //1		~0000 0110	czyli 1111 1001
		~(SEG_A|SEG_B|SEG_D|SEG_E|SEG_G),			    //2		~0101 1011	czyli 1010 0100
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_G),			    //3
		~(SEG_B|SEG_C|SEG_F|SEG_G),				        //4
		~(SEG_A|SEG_C|SEG_D|SEG_F|SEG_G),			    //5
		~(SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G),			//6
		~(SEG_A|SEG_B|SEG_C|SEG_F),				        //7
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G),	//8
		~(SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G),			//9
};

void init(void)
{
	SEGMENTY_KIERUNEK |= SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G; // (DDRA  = x111 1111) Ustawienie rejestru kierunkowego portu segmentow na wyjscie
	SEGMENTY_PORT |= SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;	  	// (PORTA = x111 1111) Wygaszenie wszystkich katod - stan wysoki

	ANODY_KIERUNEK |= ANODA_1 | ANODA_2 | ANODA_3;	// (DDRC = xxxx x111) Ustawienie anod na wyjscia
	ANODY_PORT |= ANODA_1 | ANODA_2 | ANODA_3;		// (PORTC = xxxx x111) Wygaszenie wszystkich wyswietlaczy - stan wysoki

	INIT_SWSTART; // Inicjalizacja przcisków
	INIT_SWSTOP;
	INIT_SWRESTART;

	INIT_LED;

	SW_CONFIG_PULLUP_START; // Pull UP do przycisków
	SW_CONFIG_PULLUP_STOP;
	SW_CONFIG_PULLUP_RESTART;

	// Ustawienia TIMER0
	TCCR0 |= (1<<WGM01); // Tryb CTC
	TCCR0 |= (1<<CS02) | (1<<CS00); // Preskaler = 1024
	OCR0 = 53; // Dodatkowy podzial przez 53	(11059200 Hz / 1024 / 200Hz[oczekiwana wartoœc])-1 = 53
	TIMSK |= (1<<OCIE0); // Zezwolenie na przerwanie CompareMatch

    // Ustawienia TIMER2
    TCCR2 |= (1 << CS21) | (1<<CS22); 		// prescaler na 256
	TIMSK |= (1 << TOIE2);
	TCNT2 = 5;
}

ISR(TIMER0_COMP_vect)
{
	static uint8_t licznik = 1;
	ANODY_PORT = ~licznik;

	if(licznik==1) 		SEGMENTY_PORT = (cyfry[cyfra[0]]);
	else if(licznik==2) SEGMENTY_PORT = (cyfry[cyfra[1]]);
	else if(licznik==4) SEGMENTY_PORT = (cyfry[cyfra[2]]);

	licznik <<= 1;
	if(licznik>4) licznik = 1;
}

ISR(INT0_vect)  //START //PD2
{
	odliczanie = 1; // Zezwolenie na odliczanie
	pomiar = 0; //Brak zezwolenia na pomiar ADC
}

ISR(INT1_vect)  //STOP //PD3
{
	odliczanie = 0; // Brak zezwolenia na odliczanie
}

ISR(INT2_vect)  //RESTART //PB2
{
	pomiar = 1; // Zezwolenie na pomiar ADC
	odliczanie = 0; // Brak zezwolenia na odliczanie
}

ISR(TIMER2_OVF_vect)
{
	licznik_ovf++;
	if(licznik_ovf == 125)
    {
		if(odliczanie == 1 && wartosc != 0) // Jeœli jest zezwolenie na odliczanie i zmierzona wartoœæ nie jest jeszcze zerem to:
            {
			    wartosc--; //Zacznij odejmowaæ do zera
		    }
		licznik_ovf = 0;
	}
}

int main(void)
{
	init();

	uint8_t z1, z2, z3;

    ADMUX |= ((1 <<  REFS0) | (1 << MUX0) | (1 << MUX1) | (1 << MUX2)); //wybor zrodla napiecia odniesienia (AVCC (+5V) s.214) i pinu pomiarowego ADC (ADC7, bo na pinie PA7 (s.215))

    ADCSRA |= ((1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2)); //uruchomienie przetwornika (s.216) i ustawienie czestotliwosci jego pracy (preskaler 128 (s.217))

	sei(); // Zezwolenie na globalne przerwania

	MCUCR |= (1 << ISC01); 		// INT0 na zbocze opadajace
	MCUCR |= (1 << ISC11); 		// INT1 na zbocze opadajace

	GICR |= (1 << INT0); 		// przerwanie na START //PD2
	GICR |= (1 << INT1); 		// przerwanie na STOP  //PD3
	GICR |= (1 << INT2); 		// przerwanie na RESET //PB2

    // Sprawdzenie czy dzia³a wyœwietlanie liczb
	cyfra[0] = 0;
	cyfra[1] = 8;
	cyfra[2] = 8;
	_delay_ms(2500);
	cyfra[0] = 3;
	cyfra[1] = 5;
	cyfra[2] = 9;
	_delay_ms(2500);

    //Sprawdzenie dzia³ania diody LED
    ON_LED;
    _delay_ms(2000);
    OFF_LED;

    while(1)
	{
		ADCSRA |= (1 << ADSC); // uruchamia konwersje (s.216)

		if(pomiar == 1) // Jeœli jest zezwolenie na pomiar -> wykonaj go
        {
		while(ADCSRA & (1 << ADSC)); // oczekiwanie na zakonczenie konwersji
		wartosc = ADC;             // odczyt pomiaru z przetwornika
		wartosc /= 10;
		}

		if(wartosc == 0 && pomiar == 0) // Jeœli wartoœæ pomiaru dosiêg³a zera i jest brak zezwolenia na pomiar -> uruchom diodê LED
		ON_LED;
		else
		OFF_LED;

		z1 = wartosc/100;
		if(z1<1) cyfra[0] = 0; else cyfra[0]=1;
		z2 = (wartosc-(z1*100)) / 10;
		cyfra[1] = z2;
		z3 = wartosc % 10;
		cyfra[2] = z3;
		_delay_ms(50);

	}
	return 0;
}
