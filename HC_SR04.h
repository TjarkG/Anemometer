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

#define VSOUND	343		//speed of sound in m/s
#define CORFAC	110		//correction factor in %
#define SMAX	3000	//maximum distance from the sensor in mm
#define TMAX	5000	//maximum time waiting for a pulse in µs
#define OFFSET	0		//Timer Cycles to subtract from messured time
#define F_CPU 	8000000UL	//CPU Frequency

#define TGR_HIGH	TRIG_PORT |= (1<<TRIG_PIN)
#define TGR_LOW		TRIG_PORT &= ~(1<<TRIG_PIN)

#define START_TIMER TCCR1B |= (1 << CS10)
#define STOP_TIMER TCCR1B &= ~(1 << CS10)

#ifndef ECHO
# warning "ECHO not defined for <HC_SR04.h>"
# define ECHO (PINB & 0x01)
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
long getSensorTime(void);
void getNReadings(short *nArray, unsigned char n);

volatile unsigned int pulse;

void start_Timer_1(void)
{
	TCCR1A = 0x00;
	TCCR1B = (1<<CS10);
	TCCR1C = 0x00;
	TCNT1 = 0;

	TIMSK |= (1 << ICIE1);		//set capture interrupt
	sei();
	TCCR1B |= (1 << ICES1);		//set sampling edge rissing
}

long getSensorTime(void) //returns Time in µs
{
	unsigned int out = 0;
	unsigned int pulseStart;
	unsigned int pulseEnd;

	TCNT1 = 0;					//clear counter
	TCCR1B |= (1 << ICES1);		//set sampling edge rissing
	TGR_LOW;

	while(!ECHO);
	pulseStart = pulse;
	TCCR1B &= ~(1 << ICES1);	//set sampling edge falling

	while(ECHO);
	pulseEnd = pulse;
	out = pulseEnd - pulseStart;

	TGR_HIGH;

	out -= OFFSET;
	_delay_ms(4);
	return out;
}

void getNReadings(short *nArray, unsigned char n)
{
	while( n-- > 0 )
	{
		nArray[0] = getSensorTime();
		nArray++;
	}
}

ISR(TIMER1_CAPT_vect)
{
	pulse = ICR1;
}

#endif /* HC_SR04_H_ */