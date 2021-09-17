#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define USE_CTC 0

//OCR0=255 -> MINIMUM

//definicje zmiennych do sterowania 6 kana³ami programowych PWM
//zmienne typu uint8_t, rozdzielczosc 8-bitowa
volatile uint8_t pwm1, pwm2, pwm3, pwm4, pwm5, pwm6;

int main(void)
{
	//***** SPRZÊTOWY PWM - 1 KANA£ OC0 (PB3) ******
	//ustawienie koncowki OC0 (PB3), sprzêtowy PWM jako WYjscie
	DDRB |= (1<<PB3);

	//ustawienia TIMER2 w Fast PWM
	TCCR0 |= (1<<WGM01) | (1<<WGM00); //tryb Fast PWM
	//TCCR0 |= (1<<COM01);	//clear at TOP //Clear OC0 on compare match, set OC0 at BOTTOM
	//czyli jak jest przerwanie to ustaw OC0 na 0, na poczatku OC0 jest na 1
	TCCR0 |= (1<<CS00); 	//preskaler = 1
	OCR0 = 255;				//wygaszenie diody w kanale PWM

	//***** PROGRAMOWY PWM - 6 KANA£ÓW *****
	//ustawienie pinów kana³ów programowych PWM jako WYjscia
	DDRC |= (1<<PC0) | (1<<PC1) | (1<<PC2);
	DDRD |= (1<<PD7) | (1<<PD6) | (1<<PD2);
	//wylaczenie diod LED podlaczych katodami do wyjsc
	PORTC |= (1<<PC0) | (1<<PC1) | (1<<PC2);
	PORTD |= (1<<PD7) | (1<<PD6) | (1<<PD2);

#if (USE_CTC == 1)
//ustawienia TIMER2 w tryb CTC
TCCR2 |= (1<<WGM21);	//tryb CTC
TCCR2 |= (1<<CS20); 	//preskaler = 1
OCR2 = 200;				//dodatkowy podzial czestotliwosci przez 200
TIMSK |= (1<<OCIE2);	//zezwolenie na przerwanie Compare Match
#else
//ustawienia TIMER2 w tryb NORMAL (zwykly licznik)
TCCR2 &= ~((1<<WGM21) | (1<<WGM20)); //tryb normal (ustawienie zer)
TCNT2=56;							 //inicjalizacja licznika wartoœci¹ 56
TCCR2 |= (1<<CS20); 				 //preskaler = 1
TIMSK |= (1<<TOIE2); 				 //zezwolenie na przerwanie OVF (przepelnienie)
#endif

	sei();		//odblokowanie globalne przerwañ
	uint8_t i;	//definicja zmiennej i na potrzeby pêtli for()
	while(1)
	{
		//pêtla rozjaœniaj¹ca 6 diod LED
		for(i=0; i<255; i++)
		{
			OCR0=i;	//ta dioda bedzie sie sciemniala w tej petli
			pwm1=i;
			pwm2=i;
			pwm3=i;
			pwm4=i;
			pwm5=i;
			pwm6=i;
			if(i>10) _delay_ms(3);
			else _delay_ms(15);
		}
		//calkowite rozjasnienie diody LED przez 100 ms
		OCR0 = 255;
		//rozswietlenie na maksimum przez 100ms
		pwm1 = 255;
		pwm2 = 255;
		pwm3 = 255;
		pwm4 = 255;
		pwm5 = 255;
		pwm6 = 255;
		_delay_ms(500);
		//petla stopniowo sciemniajaca 6 diod LED
		for(i=255;i;i--)
		{
			OCR0=i; //ta dioda bedzie rozjasniala sie w tej petli
			pwm1=i;
			pwm2=i;
			pwm3=i;
			pwm4=i;
			pwm5=i;
			pwm6=i;
			if(i>10) _delay_ms(3);
			else _delay_ms(15);
		}
		//calkowicie rozjasnienie diod LED przez 100ms
		OCR0=0;
		//calkowite wygaszenie diod LED przez 100ms
		pwm1 = 0;
		pwm2 = 0;
		pwm3 = 0;
		pwm4 = 0;
		pwm5 = 0;
		pwm6 = 0;
		_delay_ms(500);
    }
}

#if (USE_CTC == 1)
//cia³o procedury obs³ugi przerwania Compare Match licznika Timera2
ISR( TIMER2_COMP_vect )
{
	  static uint8_t cnt; //definicja naszego licznika PWM
	  //bezposrednie sterowanie wyjœciami kana³ów PWM
	  if(cnt>=pwm1) PORTC |= (1<<PC0); else PORTC &= ~(1<<PC0);
	  if(cnt>=pwm2) PORTC |= (1<<PC1); else PORTC &= ~(1<<PC1);
	  if(cnt>=pwm3) PORTC |= (1<<PC2); else PORTC &= ~(1<<PC2);
	  if(cnt>=pwm4) PORTD |= (1<<PD7); else PORTD &= ~(1<<PD7);
	  if(cnt>=pwm5) PORTD |= (1<<PD6); else PORTD &= ~(1<<PD6);
	  if(cnt>=pwm6) PORTD |= (1<<PD2); else PORTD &= ~(1<<PD2);
	  cnt++;
}
#else
//cia³o procedury obs³ugi przerwania od przepe³nienia licznika Timera2
ISR( TIMER2_OVF_vect )
{
	  static uint8_t cnt; //definicja naszego licznika PWM

	  //inicjalizacja licznika wartoscia 56, zeby zliczal wciaz 200 tykniec
	  TCNT2=56;

	  if(cnt>=pwm1) PORTC |= (1<<PC0); else PORTC &= ~(1<<PC0);
	  if(cnt>=pwm2) PORTC |= (1<<PC1); else PORTC &= ~(1<<PC1);
	  if(cnt>=pwm3) PORTC |= (1<<PC2); else PORTC &= ~(1<<PC2);
	  if(cnt>=pwm4) PORTD |= (1<<PD7); else PORTD &= ~(1<<PD7);
	  if(cnt>=pwm5) PORTD |= (1<<PD6); else PORTD &= ~(1<<PD6);
	  if(cnt>=pwm6) PORTD |= (1<<PD2); else PORTD &= ~(1<<PD2);

	  cnt++;
}
#endif
