/*
 * main.c  !!!!!!!!!!!! UWAGA .... w tym projekcie trzeba w³¹czyc w Eclipse
 *
 *                     ³adowanie pliku *.eep do pamiêci EEPROM przy wgrywaniu wsadu HEX
 *
 *  Created on: 2021-03-31
 *       Autor: Remigiusz Mielcarz
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "lcd44780.h"

// ponizsza linijke czasami trzeba wpisac w eclipse przed definicjami
// zmiennych w pamieci EEPROM, zeby nie podkreslalo skladni jako blednej
#define EEMEM __attribute__((section(".eeprom")))

//const char PROGMEM tab1[] = {"FLASH"};
const char PROGMEM tab1[] = {"REMIGIUSZ"};
char EEMEM tab2[] = {"EEPROM"};

uint8_t znak_L[] = {16,16,18,20,24,16,31,0};		// wzór znaku litery FUNT w pamieci RAM
uint8_t znak_euro[] = {7,8,30,8,30,8,7,0};
uint8_t znak_dolar[] = {4,15,20,20,14,5,30,4};
uint8_t znak_o[] EEMEM = {4,32,14,17,17,17,14,0};	// wzór znaku litery ó w pamieci EEPROM
const uint8_t znak_buzka[] PROGMEM = {14,17,27,17,17,21,17,14};	// wzór znaku buYki w pamieci FLASH
const uint8_t znak_termo[] PROGMEM = {4,10,10,10,17,31,31,14};	// wzór znaku termometru w pamieci FLASH

volatile uint8_t pwm1, pwm2, pwm3;

int main(void)
{
	DDRA |= (1<<PA7);	// ustawiamy kierunek linii podswietlenia LCD jako WYJSCIE
	PORTA |= (1<<PA7);	// zalaczamy podswietlenie LCD - stan wysoki

	lcd_init();

	/*
	lcd_str_P(tab1);				// napis z pamieci FLASH
	lcd_locate(0,10);
	lcd_str_P( PSTR("Linia1") );	// napis z pamieci FLASH
	lcd_locate(1,0);
	lcd_str_E(tab2);				// napis z pamieci EEPROM
	lcd_locate(1,10);
	lcd_str("Linia2");				// napis z pamieci RAM

	// zaladowanie znaków do pamieci CGRAM
	lcd_defchar(0x80, znak_L);
	lcd_defchar_E(0x81, znak_o);
	lcd_defchar_P(0x82, znak_buzka);
	lcd_defchar_P(0x83, znak_termo);


	// wyswietlenie wlasnych znaków na LCD
	lcd_locate(0,7);
	lcd_str("\x80\x81");
	lcd_locate(1,8);
	lcd_str("\x82");
	lcd_locate(1,7);
	lcd_str("\x83");
	_delay_ms(1500);

	uint8_t a=0;
	while(1) {
		lcd_locate(1,0);
		lcd_int( a++ );
		lcd_str( "  " );
		_delay_ms(250);
	}
	*/

	// zaladowanie znaków do pamieci CGRAM
	lcd_defchar(0x80, znak_euro);
	lcd_defchar(0x81, znak_dolar);

	lcd_locate(1,14);
	lcd_str("\x80");

	lcd_locate(1,15);
	lcd_str("\x81");

	lcd_locate(0,0);
	lcd_str_P(tab1);				// napis z pamieci FLASH
	lcd_locate(1,0);
	lcd_str_P( PSTR("MIELCARZ") );	// napis z pamieci FLASH
	lcd_locate(0,10);
	lcd_str("LAT");					// napis z pamieci RAM

	_delay_ms(1500);

	uint8_t a=0;
	while(1) {
		lcd_locate(0,14);
		lcd_int( a++ );
		lcd_str( "  " );
		if(a==21) break;
		_delay_ms(250);
	}

	//_delay_ms(2000);
	//lcd_cls();
}

