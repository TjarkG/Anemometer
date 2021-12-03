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

short getSensorHight(void);
long getSensorTime(void);
void getNReadings(short *nArray, unsigned char n);
void quicksort(short* data, unsigned char n);
unsigned short getAverageReding(const unsigned char n);
unsigned short getMedianReding(const unsigned char n);

short getSensorHight(void) //returns Distance in mm
{
	TGR_LOW;
	_delay_us(10);
	
	unsigned short i = 0;
	while( !ECHO && i++ <= TMAX )
		_delay_us(1);
	
	unsigned long out = 0;
	while( ECHO && out++ < SMAX )
		_delay_us(9);
	
	out = (out*VSOUND*CORFAC)/20000;		//convert m/s to mm per 10 µs, divides by 2 and applies a Correction Factor ( out *= ((VSOUND/200.0)*CORFAC) )
	TGR_HIGH;
	_delay_ms(15);
	return (i>=TMAX || out>SMAX)? -1 : out;
}

void start_Timer_1(void)
{
	TCCR1A = 0x00;
	TCCR1B = (1<<CS10);
	TCCR1C = 0x00;
	TCNT1 = 0;

	TIMSK |= (1 << ICIE1);		//Set capture interrupt
	sei();
	TCCR1B |= (1 << ICES1);		//Set capture rising edge
}

volatile unsigned int pulseStart;
volatile unsigned int pulseEnd;
volatile unsigned int pulseInUs;
volatile unsigned char edge = 0;

long getSensorTime(void) //returns Time in µs
{
	unsigned short out = 0;
	TCNT1 = 0;
	edge = 0;
	TGR_LOW;

	while(!ECHO);
	edge = 1;

	while(ECHO);
	out = pulseInUs;

	TGR_HIGH;

	out -= OFFSET;
	_delay_ms(4);
	return out;
}

ISR(TIMER1_CAPT_vect){
	if(edge == 0){
		pulseStart = ICR1;					//copy capture value
		TCCR1B &= ~(1 << ICES1);			//toggle capture edge						
	}
	else{
		pulseEnd = ICR1;					//copy capture value
		TCCR1B |= (1 << ICES1);				//toggle capture edge
		pulseInUs = pulseEnd - pulseStart;
	}
}

void getNReadings(short *nArray, unsigned char n)
{
	while( n-- > 0 )
	{
		nArray[0] = getSensorTime();
		nArray++;
	}
}

void quicksort(short* data, unsigned char n)       //sorts data till n ascending
{
  if (n < 2) return;
 
  int pivot = data[n / 2];
 
  int i, j;
  for (i = 0, j = n - 1; ; i++, j--)
  {
    while (data[i] < pivot) i++;
    while (data[j] > pivot) j--;
 
    if (i >= j) break;
 
    int temp = data[i];
    data[i]     = data[j];
    data[j]     = temp;
  }
 
  quicksort(data, i);
  quicksort(data + i, n - i);
}

unsigned short getAverageReding(const unsigned char n)
{
	short readings[n];
	getNReadings(readings, n);
	
	unsigned long average = 0;
	for (unsigned char i = 0; i<n; i++)
		average += readings[i];
	average /= n;
	return average;
}

unsigned short getMedianReding(const unsigned char n)
{
	short readings[n];
	getNReadings(readings, n);
	quicksort(readings, n);
	return n % 2 ? readings[n / 2] : (readings[n / 2 - 1] + readings[n / 2]) / 2;
}

#endif /* HC_SR04_H_ */