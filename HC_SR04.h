/*
 * HC_SR04.h
 *
 * Header to read the distance from a HC-SR04 ultrasonic sensor
 *
 * Created: 04.07.2021 16:39:57
 *  Author: tjark
 */ 

#ifndef HC_SR04_H_
#define HC_SR04_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define TMAX	10000	//maximum time waiting for a pulse in µs
#define OFFSET	0		//Timer Cycles to subtract from messured time

#define TGR_HIGH	TRIG_PORT |= (1<<TRIG_PIN)
#define TGR_LOW		TRIG_PORT &= ~(1<<TRIG_PIN)

#define START_TIMER TCCR1B |= (1 << CS10)
#define STOP_TIMER TCCR1B &= ~(1 << CS10)

#ifndef TIME
# warning "TIME not defined for <HC_SR04.h>"
# define TIME (PINB & 0x01)
#endif

#ifndef TRIG_PORT
# warning "TRIG_PORT not defined for <HC_SR04.h>"
# define TRIG_PORT PORTB
#endif

#ifndef TRIG_PIN
# warning "TRIG_PIN not defined for <HC_SR04.h>"
# define TRIG_PIN PINB1
#endif

void start_Timer_1(void);
int getSensorTime(void);

void start_Timer_1(void)
{
	TGR_HIGH;
	
	TCCR1A = 0x00;
	TCCR1B = (1<<CS11) | (1<<CS12) | (1<<CS10);
	TCCR1C = 0x00;
	TCNT1 = 0;
}

int getSensorTime(void) //returns Time in something
{
	unsigned int i = 0;
	unsigned int out = 0;
	TGR_LOW;
	_delay_us(10);

	while( !ECHO && i++ < TMAX )
		_delay_us(1);
	while( ECHO && out++ < TMAX )
		_delay_us(1);
		
	TGR_HIGH;
	_delay_ms(15);
	return (i>=(TMAX-1) || out>=(TMAX-1))? 0 : out;
}

#endif /* HC_SR04_H_ */