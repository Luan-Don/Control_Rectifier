/*
 * myLCD.h
 *
 * Created: 3/8/2022 10:22:08 AM
 *  Author: khueh
 */


#ifndef MYLCD_H_
#define MYLCD_H_

#include <avr/io.h>
//#include <util/delay.h>
#include <stdio.h>

#define   sbi(sfr,bit)  sfr|=_BV(bit)
#define   cbi(sfr,bit)  sfr&=~(_BV(bit))

#define LCD_NO_COLS     16
#define LCD_NO_ROWS     2

#define EN        2
#define RW        1
#define RS        0
#define CTRL      PORTA
#define DDR_CTRL  DDRA

#define DATA_O    PORTA
#define DATA_I    PINA
#define DDR_DATA  DDRA

/*
#define LCD8BIT
#define DATA_O    PORTD
#define DATA_I    PIND
#define DDR_DATA  DDRD
*/

char Read2Nib(void);

void Write2Nib(uint8_t chr);

void Write8Bit(uint8_t chr);

void wait_LCD(void);

void init_LCD(void);

void home_LCD(void);

void move_LCD(uint8_t y,uint8_t x);

void clr_LCD(void);

void putChar_LCD(uint8_t chr);

void putStr_LCD(const char* str);

void printf_LCD(const char *format, ...);

#endif /* MYLCD_H_ */
