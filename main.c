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
#define Nr_Med      256          //Number of readings used when setting offset

#define TIME    (PIND & (1 << 6))
#define PULS    (PIND & (1 << 5))
#define PULS2   (PIND & (1 << 4))
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

    while(1)
    {
        //Set offsets if button is pressed
        if(!SET0)
        {
            unsigned long avg[Nr_Sens] = {0};
            for (unsigned int i = 0; i < Nr_Med; i++)
            {
                for (unsigned char j = 0; j < Nr_Sens; j++)
                {
                    setAdr(j);
                    avg[j] += getSensorTime();
                }
            }
            for (unsigned char i = 0; i < (Nr_Sens/2); i++)
            {
                writeOfs(i, velocity(avg[i*2]/Nr_Med, avg[(i*2)+1]/Nr_Med));
            }
        }
        //take regular messurment
        unsigned int time[Nr_Sens];
        for (unsigned char i = 0; i < Nr_Sens; i++)
        {
            setAdr(i);
            time[i] = getSensorTime();
        }
        for (unsigned char i = 0; i < Nr_Sens; i += 2)
        {
            uartWriteInt(velocity(time[i], time[i+1])-readOfs(i/2));
        }   
        uartWriteString("\n\r");
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