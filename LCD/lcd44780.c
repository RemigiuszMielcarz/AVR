//-----------------------------------------------------------------------------------------------------------
// *** Obs³uga wyœwietlaczy alfanumerycznych zgodnych z HD44780 ***
//
// - Sterowanie: tryb 4-bitowy
// - Dowolne przypisanie ka¿dego sygna³u steruj¹cego do dowolnego pinu mikrokontrolera
// - Praca z pinem RW pod³¹czonym do GND lub do mikrokontrolera (sprawdzanie BusyFLAG - szybkie operacje LCD)
//----------------------------------------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>

#include "lcd44780.h"

// makrodefinicje operacji na sygna³ach steruj¹cych RS,RW oraz E

#define SET_RS 	PORT(LCD_RSPORT) |= (1<<LCD_RS)			// stan wysoki na linii RS
#define CLR_RS 	PORT(LCD_RSPORT) &= ~(1<<LCD_RS)		// stan niski na linii RS

#define SET_RW 	PORT(LCD_RWPORT) |= (1<<LCD_RW)			// stan wysoki na RW - odczyt z LCD
#define CLR_RW 	PORT(LCD_RWPORT) &= ~(1<<LCD_RW)		// stan niski na RW - zapis do LCD

#define SET_E 	PORT(LCD_EPORT) |= (1<<LCD_E)			// stan wysoki na linii E
#define CLR_E 	PORT(LCD_EPORT) &= ~(1<<LCD_E)			// stan niski na linii E


uint8_t check_BF(void);			// deklaracja funkcji wewnêtrznej


//********************* FUNKCJE WEWNÊTRZNE *********************

//----------------------------------------------------------------------------------------
//
//		 Ustawienie wszystkich 4 linii danych jako WYjœcia
//
//----------------------------------------------------------------------------------------
static inline void data_dir_out(void)
{
	DDR(LCD_D7PORT)	|= (1<<LCD_D7);
	DDR(LCD_D6PORT)	|= (1<<LCD_D6);
	DDR(LCD_D5PORT)	|= (1<<LCD_D5);
	DDR(LCD_D4PORT)	|= (1<<LCD_D4);
}

//----------------------------------------------------------------------------------------
//
//		 Ustawienie wszystkich 4 linii danych jako Wejœcia
//
//----------------------------------------------------------------------------------------
static inline void data_dir_in(void)
{
	DDR(LCD_D7PORT)	&= ~(1<<LCD_D7);
	DDR(LCD_D6PORT)	&= ~(1<<LCD_D6);
	DDR(LCD_D5PORT)	&= ~(1<<LCD_D5);
	DDR(LCD_D4PORT)	&= ~(1<<LCD_D4);
}

//----------------------------------------------------------------------------------------
//
//		 Wys³anie po³ówki bajtu do LCD (D4..D7)
//
//----------------------------------------------------------------------------------------
static inline void lcd_sendHalf(uint8_t data)
{
	if (data&(1<<0)) PORT(LCD_D4PORT) |= (1<<LCD_D4); else PORT(LCD_D4PORT) &= ~(1<<LCD_D4);
	//jezeli jest jedynka na na najmlodszym pinie daty -> ustaw jedynke na D4
	//if (data&(1<<0) PORTA |= (1<<3)
	if (data&(1<<1)) PORT(LCD_D5PORT) |= (1<<LCD_D5); else PORT(LCD_D5PORT) &= ~(1<<LCD_D5);
	if (data&(1<<2)) PORT(LCD_D6PORT) |= (1<<LCD_D6); else PORT(LCD_D6PORT) &= ~(1<<LCD_D6);
	if (data&(1<<3)) PORT(LCD_D7PORT) |= (1<<LCD_D7); else PORT(LCD_D7PORT) &= ~(1<<LCD_D7);
}

#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Odczyt po³ówki bajtu z LCD (D4..D7)
//
//----------------------------------------------------------------------------------------
static inline uint8_t lcd_readHalf(void)
{
	uint8_t result=0;

	if(PIN(LCD_D4PORT)&(1<<LCD_D4)) result |= (1<<0);
	if(PIN(LCD_D5PORT)&(1<<LCD_D5)) result |= (1<<1);
	if(PIN(LCD_D6PORT)&(1<<LCD_D6)) result |= (1<<2);
	if(PIN(LCD_D7PORT)&(1<<LCD_D7)) result |= (1<<3);
	//jezeli na pinie D7 jest jedynka to przypisz result jedynke na PR3
	//nie przypisujemy zer bo caly bajt result jest ustawiony ju¿ na 0
	return result; //zwroc caly result w postaci 8-bitowej
}
#endif

//----------------------------------------------------------------------------------------
//
//		 Zapis bajtu do wyœwietlacza LCD
//
//----------------------------------------------------------------------------------------
void _lcd_write_byte(unsigned char _data)
{
	// Ustawienie pinów portu LCD D4..D7 jako wyjœcia
	data_dir_out();

#if USE_RW == 1
	CLR_RW; //dlatego, bo do zapisu RW musi byc i tak 0
#endif

	SET_E;
	lcd_sendHalf(_data >> 4);			// wys³anie starszej czêœci bajtu danych D7..D4
	CLR_E;								// przesun date w prawo o 4 miejsca

	SET_E;
	lcd_sendHalf(_data);				// wys³anie m³odszej czêœci bajtu danych D3..D0
	CLR_E;

#if USE_RW == 1
	while( (check_BF() & (1<<7)) ); 	// sprawdz zajetosc flagi na najstarszym pinie PD7
#else
	_delay_us(120);
#endif

}

#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Odczyt bajtu z wyœwietlacza LCD
//
//----------------------------------------------------------------------------------------
uint8_t _lcd_read_byte(void)
{
	uint8_t result=0;
	data_dir_in();

	SET_RW;	//bo do odczytu i tak RW musi byc 1

	SET_E;
	result = (lcd_readHalf() << 4);	// odczyt starszej czêœci bajtu z LCD D7..D4
	CLR_E;

	SET_E;
	result |= lcd_readHalf();			// odczyt m³odszej czêœci bajtu z LCD D3..D0
	CLR_E;

	return result;
}
#endif


#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Sprawdzenie stanu Busy Flag (Zajêtoœci wyœwietlacza)
//
//----------------------------------------------------------------------------------------
uint8_t check_BF(void)
{
	CLR_RS; //KOMENDA RS=0
	return _lcd_read_byte();
}
#endif


//----------------------------------------------------------------------------------------
//
//		 Zapis komendy do wyœwietlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_cmd(uint8_t cmd)
{
	CLR_RS; //KOMENDA RS=0
	_lcd_write_byte(cmd);
}

//----------------------------------------------------------------------------------------
//
//		 Zapis danych do wyœwietlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_data(uint8_t data)
{
	SET_RS; //ZNAKI DO WYŒW. RS=1
	_lcd_write_byte(data);
}




//**************************  FUNKCJE PRZEZNACZONE TAK¯E DLA INNYCH MODU£ÓW  ******************

#if USE_LCD_CHAR == 1
//----------------------------------------------------------------------------------------
//
//		 Wys³anie pojedynczego znaku do wyœwietlacza LCD w postaci argumentu
//
//		 8 w³asnych znaków zdefiniowanych w CGRAM
//		 wysy³amy za pomoc¹ kodów 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_char(char c)
{
	lcd_write_data( ( c>=0x80 && c<=0x87 ) ? (c & 0x07) : c);
}
#endif

//----------------------------------------------------------------------------------------
//
//		 Wys³anie stringa do wyœwietlacza LCD z pamiêci RAM
//
//
//----------------------------------------------------------------------------------------
void lcd_str(char * str)
{
	register char znak;
	while ( (znak=*(str++)) ) lcd_char( znak );
}

#if USE_LCD_STR_P == 1
//----------------------------------------------------------------------------------------
//
//		 Wys³anie stringa do wyœwietlacza LCD z pamiêci FLASH
//
//----------------------------------------------------------------------------------------
void lcd_str_P(const char * str)
{
	register char znak;
	while ( (znak=pgm_read_byte(str++)) ) lcd_char( znak );
}
#endif


#if USE_LCD_STR_E == 1
//----------------------------------------------------------------------------------------
//
//		 Wys³anie stringa do wyœwietlacza LCD z pamiêci EEPROM
//
//		 8 w³asnych znaków zdefiniowanych w CGRAM
//		 wysy³amy za pomoc¹ kodów 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_str_E(char * str)
{
	register char znak;
	while(1)
	{
		znak=eeprom_read_byte( (uint8_t *)(str++) );
		if(!znak || znak==0xFF) break;
		else lcd_char( znak );
	}
}
#endif


#if USE_LCD_INT == 1
//----------------------------------------------------------------------------------------
//
//		 Wyœwietla liczbê dziesiêtn¹ na wyœwietlaczu LCD
//
//----------------------------------------------------------------------------------------
void lcd_int(int val)
{
	char bufor[17];
	lcd_str( itoa(val, bufor, 10) );
}
#endif

#if USE_LCD_HEX == 1
//----------------------------------------------------------------------------------------
//
//		 Wyœwietla liczbê szestnastkow¹ HEX na wyœwietlaczu LCD
//
//----------------------------------------------------------------------------------------
void lcd_hex(uint32_t val)
{
	char bufor[17];
	lcd_str( ltoa(val, bufor, 16) );
}
#endif

#if USE_LCD_DEFCHAR == 1
//----------------------------------------------------------------------------------------
//
//		Definicja w³asnego znaku na LCD z pamiêci RAM
//
//		argumenty:
//		nr: 		- kod znaku w pamiêci CGRAM od 0x80 do 0x87
//		*def_znak:	- wskaŸnik do tablicy 7 bajtów definiuj¹cych znak
//
//----------------------------------------------------------------------------------------
void lcd_defchar(uint8_t nr, uint8_t *def_znak)
{
	register uint8_t i,c;
	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++)
	{
		c = *(def_znak++);
		lcd_write_data(c);
	}
}
#endif

#if USE_LCD_DEFCHAR_P == 1
//----------------------------------------------------------------------------------------
//
//		Definicja w³asnego znaku na LCD z pamiêci FLASH
//
//		argumenty:
//		nr: 		- kod znaku w pamiêci CGRAM od 0x80 do 0x87
//		*def_znak:	- wskaŸnik do tablicy 7 bajtów definiuj¹cych znak
//
//----------------------------------------------------------------------------------------
void lcd_defchar_P(uint8_t nr, const uint8_t *def_znak)
{
	register uint8_t i,c;
	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++)
	{
		c = pgm_read_byte(def_znak++);
		lcd_write_data(c);
	}
}
#endif

#if USE_LCD_DEFCHAR_E == 1
//----------------------------------------------------------------------------------------
//
//		Definicja w³asnego znaku na LCD z pamiêci EEPROM
//
//		argumenty:
//		nr: 		- kod znaku w pamiêci CGRAM od 0x80 do 0x87
//		*def_znak:	- wskaŸnik do tablicy 7 bajtów definiuj¹cych znak
//
//----------------------------------------------------------------------------------------
void lcd_defchar_E(uint8_t nr, uint8_t *def_znak)
{
	register uint8_t i,c;

	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++)
	{
		c = eeprom_read_byte(def_znak++);
		lcd_write_data(c);
	}
}
#endif


#if USE_LCD_LOCATE == 1
//----------------------------------------------------------------------------------------
//
//		Ustawienie kursora w pozycji Y-wiersz, X-kolumna
//
// 		Y = od 0 do 3
// 		X = od 0 do n
//
//		funkcja dostosowuje automatycznie adresy DDRAM
//		w zale¿noœci od rodzaju wyœwietlacza (ile posiada wierszy)
//
//----------------------------------------------------------------------------------------
void lcd_locate(uint8_t y, uint8_t x)
{
	switch(y)
	{
		case 0: y = LCD_LINE1; break;

#if (LCD_ROWS>1)
	    case 1: y = LCD_LINE2; break; // adres 1 znaku 2 wiersza
#endif
#if (LCD_ROWS>2)
    	case 2: y = LCD_LINE3; break; // adres 1 znaku 3 wiersza
#endif
#if (LCD_ROWS>3)
    	case 3: y = LCD_LINE4; break; // adres 1 znaku 4 wiersza
#endif
	}

	lcd_write_cmd( (0x80 + y + x) );
}
#endif


//----------------------------------------------------------------------------------------
//
//		Kasowanie ekranu wyœwietlacza
//
//----------------------------------------------------------------------------------------
void lcd_cls(void)
{
	lcd_write_cmd( LCDC_CLS );

	#if USE_RW == 0
		_delay_ms(4.9);
	#endif
}


#if USE_LCD_CURSOR_HOME == 1
//----------------------------------------------------------------------------------------
//
//		Powrót kursora na pocz¹tek
//
//----------------------------------------------------------------------------------------
void lcd_home(void)
{
	lcd_write_cmd( LCDC_CLS|LCDC_HOME );

	#if USE_RW == 0
		_delay_ms(4.9);
	#endif
}
#endif

#if USE_LCD_CURSOR_ON == 1
//----------------------------------------------------------------------------------------
//
//		W³¹czenie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_cursor_on(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON);
}

//----------------------------------------------------------------------------------------
//
//		Wy³¹czenie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_cursor_off(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON);
}
#endif


#if USE_LCD_CURSOR_BLINK == 1
//----------------------------------------------------------------------------------------
//
//		W£¹cza miganie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_blink_on(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON|LCDC_BLINKON);
}

//----------------------------------------------------------------------------------------
//
//		WY³¹cza miganie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_blink_off(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON);
}
#endif




//----------------------------------------------------------------------------------------
//
//		 ******* INICJALIZACJA WYŒWIETLACZA LCD ********
//
//----------------------------------------------------------------------------------------
void lcd_init(void)
{
	// inicjowanie pinów portów ustalonych do pod³¹czenia z wyœwietlaczem LCD
	// ustawienie wszystkich jako wyjœcia
	data_dir_out();
	DDR(LCD_RSPORT) |= (1<<LCD_RS);
	DDR(LCD_EPORT) |= (1<<LCD_E);
	#if USE_RW == 1
		DDR(LCD_RWPORT) |= (1<<LCD_RW);
	#endif

	PORT(LCD_RSPORT) |= (1<<LCD_RS);
	PORT(LCD_EPORT) |= (1<<LCD_E);
	#if USE_RW == 1
		PORT(LCD_RWPORT) |= (1<<LCD_RW);
	#endif

	_delay_ms(15);
	PORT(LCD_EPORT) &= ~(1<<LCD_E);
	PORT(LCD_RSPORT) &= ~(1<<LCD_RS);
	#if USE_RW == 1
    	PORT(LCD_RWPORT) &= ~(1<<LCD_RW);
	#endif

	// jeszcze nie mo¿na u¿ywaæ Busy Flag
	SET_E;
	lcd_sendHalf(0x03);	// tryb 8-bitowy DL = 1
	CLR_E;
	_delay_ms(4.1);

	SET_E;
	lcd_sendHalf(0x03);	// tryb 8-bitowy DL = 1
	CLR_E;
	_delay_us(100);

	SET_E;
	lcd_sendHalf(0x03);	// tryb 8-bitowy DL = 1
	CLR_E;
	_delay_us(100);

	SET_E;
	lcd_sendHalf(0x02);// tryb 4-bitowy DL = 0
	CLR_E;
	_delay_us(100);

	// ju¿ mo¿na u¿ywaæ Busy Flag
	// tryb 4-bitowy, 2 wiersze, znak 5x7
	lcd_write_cmd( LCDC_FUNC|LCDC_FUNC4B|LCDC_FUNC2L|LCDC_FUNC5x7 );
	// wy³¹czenie kursora
	lcd_write_cmd( LCDC_ONOFF|LCDC_CURSOROFF );
	// w³¹czenie wyœwietlacza
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON );
	// przesuwanie kursora w prawo bez przesuwania zawartoœci ekranu
	lcd_write_cmd( LCDC_ENTRY|LCDC_ENTRYR );

	// kasowanie ekranu
	lcd_cls();
}
