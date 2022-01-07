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

#define SMAX	5000	//maximum distance from the sensor in mm
#define TMAX	10000	//maximum time waiting for a pulse in µs

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

#define TGR_HIGH	TRIG_PORT |= (1<<TRIG_PIN)
#define TGR_LOW		TRIG_PORT &= ~(1<<TRIG_PIN)

short getSensorHight(void) //returns Distance in mm
{
	unsigned int i = 0;
	unsigned int out = 0;
	TGR_LOW;
	_delay_us(10);

	while( !ECHO && i++ < TMAX );
	
	while( ECHO && out++ < SMAX );
		
	TGR_HIGH;
	_delay_ms(15);
	return (i>=(TMAX-1) || out>=(SMAX-1))? (-1) : out;
}

void getNReadings(short *nArray, unsigned char n)
{
	while( n-- > 0 )
	{
		nArray[0] = getSensorHight();
		nArray++;
	}
}

void quicksort(short *data, unsigned char n)
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

short getMedianReding(const unsigned char n)
{
	short readings[n];
	getNReadings(readings, n);
	if(readings[0] == -1)
		return -1;
	quicksort(readings, n);
	short temp = n % 2 ? readings[n / 2] : (readings[n / 2 - 1] + readings[n / 2]) / 2;
	return (temp*100L)/520;		//Conversion Factor from Spreadsheet
}

#endif /* HC_SR04_H_ */