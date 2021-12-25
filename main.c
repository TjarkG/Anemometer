/*
 * main.c for ultrasonic anemometer
 *
 * Created: 22.11.2021 13:52
 * Author : tjark
 * Target : Anemometer.sch
 * 
 * Fuses: 
 * avrdude -U lfuse:w:0xff:m -U hfuse:w:0x9b:m -U efuse:w:0xff:m -c avrispmkII -p attiny2313
 */

#ifndef __AVR_ATtiny2313A__
#define __AVR_ATtiny2313A__
#endif

#define F_CPU       16000000UL
#define S_SENSOR    200         //Distance betwen Sensors in mm
#define Nr_Sens     2           //Number of Sensors connected

#define TIME    (PIND & (1 << 6))
#define PULS    (PIND & (1 << 5))
#define PULS2   (PIND & (1 << 4))
#define TRIG_PORT PORTD
#define TRIG_PIN PIND3

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "HC_SR04.h"
#include "uart.h"

void setAdr(char adr);
int velocity(unsigned int t1, unsigned int t2);

int main(void)
{
    DDRD = (1 << 1) | (1 << 3);
    DDRB = (1 << 0) | (1 << 1) | (1 << 2);
    uartInit();
    start_Timer_1();        //Timer for Messuring Sensor Delay

    while(1)
    {
        unsigned int time[Nr_Sens];
        for (unsigned char i = 0; i < Nr_Sens; i++)
        {
            setAdr(i);
            time[i] = getSensorTime();
        }
        
        //v = d/t
        //long velocity = S_SENSOR/(time*1000);
        uartWriteInt(velocity(time[0], time[1]));
        //uartWriteIntLine(abs(((int)time[1]) - ((int)time[0])));

        uartWriteIntArray(time, Nr_Sens);
        _delay_ms(50);
    }
}

void setAdr(char adr)
{
    unsigned char tmp = PINB;
    tmp &= ~(0x07);
    tmp |= ((!!(adr & (1<<2))) << 0) | ((!!(adr & (1<<1))) << 1) | ((!!(adr & (1<<0))) << 2);
    PORTB = tmp;
}

int velocity(unsigned int t1, unsigned int t2)  //returns velocity in cm/s
{
    return ((S_SENSOR*(F_CPU/100))/t1) - ((S_SENSOR*(F_CPU/100))/t2);
}