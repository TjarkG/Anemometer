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
#include <util/delay.h>

#define VSOUND	343		//speed of sound in m/s
#define CORFAC	110		//correction factor in %
#define SMAX	3000	//maximum distance from the sensor in mm
#define TMAX	5000	//maximum time waiting for a pulse in �s

#define TGR_HIGH	TRIG_PORT |= (1<<TRIG_PIN)
#define TGR_LOW		TRIG_PORT &= ~(1<<TRIG_PIN)

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
	
	out = (out*VSOUND*CORFAC)/20000;		//convert m/s to mm per 10 �s, divides by 2 and applies a Correction Factor ( out *= ((VSOUND/200.0)*CORFAC) )
	TGR_HIGH;
	_delay_ms(15);
	return (i>=TMAX || out>SMAX)? -1 : out;
}

void getNReadings(short *nArray, unsigned char n)
{
	while( n-- > 0 )
	{
		nArray[0] = getSensorHight();
		nArray++;
	}
}

void linSort(short* data, unsigned char length)
{
	for (unsigned char i = 0; i < length; i++)
		for (unsigned char j = 0; j < length-i-1; j++)
			if (data[j] > data[j + 1])
			{
				short temp = data[j];
				data[j] = data[j + 1];
				data[j + 1] = temp;
			}
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
	linSort(readings, n);
	return n % 2 ? readings[n / 2] : (readings[n / 2 - 1] + readings[n / 2]) / 2;
}

#endif /* HC_SR04_H_ */