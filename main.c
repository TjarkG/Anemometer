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
#define Nr_Sens     1           //Number of Sensors connected
#define Nr_Med      128         //Number of readings used when setting offset
#define ASCII                   //Comment out for raw output

#define TIME    (PIND & (1 << 6))
#define PULS    (PIND & (1 << 5))
#define PULS2   (PIND & (1 << 2))
#define PULS4   (PIND & (1 << 4))
#define ECHO    (PINB & (1 << 3))
#define TRIG_PORT PORTD
#define TRIG_PIN PIND3

#define SET0    (PINB & (1 << 4))

#define writeOfs(adr, value)    eeprom_write_word((uint16_t*) (unsigned int) (adr), (value) )
#define readOfs(i)              ((int) eeprom_read_word((uint16_t*) (unsigned int) (i)))

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "HC_SR04.h"
#include "uart.h"

void setAdr(char adr);
int velocity(unsigned int t1, unsigned int t2);

int main(void)
{
    DDRD = (1 << 1) | (1 << 3);
    DDRB = (1 << 0) | (1 << 1) | (1 << 2);
    PORTB |= (1 << PINB4);
    uartInit();
    start_Timer_1();        //Timer for Messuring Sensor Delay
    setAdr(0);

    while(1)
    {
        int time;
        time = getSensorTime();
        uartWriteIntLine(time);
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