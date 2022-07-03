/*
 * main.c
 *
 *  Created on: May 28, 2022
 *      Author: luand
 */
#include <math.h>
#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>
#define F_CPU 7372800UL
#include <util/delay.h>
#include "myLCD.h"
#define CTRL_ANGLE_DDR	DDRD
#define CTRL_ANGLE_PORT PORTD
#define DEC_ANGLE		0
#define INC_ANGLE		1
#define CHECK_AC_DDR	DDRE
#define CHECK_AC_PORT	PORTE
#define THY1			2
#define THY2			3
//#define POS_HALF_TERM	4
#define NEG_HALF_TERM	4
#define HALF_CYCLE		10
unsigned int angle_kicking_pulse = 30;
unsigned int time_kicking = 1000;
const float gain = 100.0/M_PI;
volatile int count = 0;
void init_IO(){
	CTRL_ANGLE_DDR &= ~(1 << DEC_ANGLE) | (1 << INC_ANGLE); 	// input interrupt: angle kicking pulse controller
	CTRL_ANGLE_PORT |= (1 << DEC_ANGLE) | (1 << INC_ANGLE);		// Pullup resistor
	EIMSK |= (1 << INT0) | (1 << INT1);							// Enable interrupt 0,1
	EICRA |= (1 << ISC11) | (1 << ISC01);						// Falling edge interrupt
	CHECK_AC_DDR |= (1 << THY1) | (1 << THY2); 					// output on Thyristor 1, 2
	CHECK_AC_DDR &= ~(1 << NEG_HALF_TERM); 		// input interrupt
	CHECK_AC_PORT |= (1 << NEG_HALF_TERM);		// Pullup resistors
	EIMSK |= (1 << INT4);						// Enable interrupt 4
	EICRB |= (1 << ISC41); 						// Falling edge on INT4
}

int main() {
	sei();	// Global interrupt
	init_IO();
	// Using LCD 16x2
	init_LCD();

	DDRE |= (1<<PE7);
	PORTE |= (1<<PE7);
	while (1) {
		clr_LCD();
		move_LCD(1,1);
		//printf_LCD("ControlRectifier");
		printf_LCD("CountINT: %d",count);
		//printf_LCD("Time: %u",time_kicking);
		move_LCD(2,1);
		printf_LCD("AngleKick: %u", angle_kicking_pulse);
		//printf_LCD("Gain: %0.2f",gain);
		_delay_ms(300);
	};
	return 0;
}
ISR(INT0_vect){
	angle_kicking_pulse++; // Increase angle
	if(angle_kicking_pulse > 90){
		angle_kicking_pulse = 90;
	}
	time_kicking = angle_kicking_pulse*gain;
}
ISR(INT1_vect){
	angle_kicking_pulse--; // Decrease angle
	if(angle_kicking_pulse < 30){
		angle_kicking_pulse = 30;
	}
	time_kicking = angle_kicking_pulse*gain;
}
// Soure 24VAC 50Hz =>> 1 cycle = 0.02s or 20ms or half_term cycle = 0.01s = 10ms
// If kicking angle is 30degree = pi/6 = 0.01s/6 = 0.00167s
// The angle depends on the Thyristor kicking time
ISR(INT4_vect) { // Possitive half term sinosoid
	for(int i = 0; i < time_kicking; i++){
		_delay_us(1);
	}
	CHECK_AC_PORT &= ~(1<<THY2);  	// OFF Thyristor 2
	CHECK_AC_PORT |= (1<<THY1);		// ON  Thyristor 1
	_delay_ms(1);
	CHECK_AC_PORT &= ~(1<<THY1);  	// OFF Thyristor 1 after 1ms
	_delay_ms(9);
	CHECK_AC_PORT |= (1<<THY2);		// ON  Thyristor 2
	_delay_ms(1);
	CHECK_AC_PORT &= ~(1<<THY2);  	// OFF Thyristor 2 after 1ms
	count++;
	if(count > 10000){
		count = 0;
	}
}
