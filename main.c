/*
 * main.c for ultrasonic anemometer
 *
 * Created: 22.11.2021 13:52
 * Author : tjark
 * Target : Anemometer.sch
 * 
 * Fuses: 
 * avrdude -U lfuse:w:0xe4:m -U hfuse:w:0x9b:m -U efuse:w:0xff:m -c avrispmkII -p attiny2313
 */

#ifndef __AVR_ATtiny2313A__
#define __AVR_ATtiny2313A__
#endif

#define F_CPU 8000000UL
#define S_SENSOR 200        //Distance betwen Sensors in mm

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

int main(void)
{
    DDRD = (1 << 3) | (1 << 1);
    uartInit();
    start_Timer_1();        //Timer for Messuring Sensor Delay

    while(1)
    {
        long time = getSensorTime();
        if(time >= 0)
        {
            //v = d/t
            //long velocity = S_SENSOR/(time*1000);
            //uartWriteIntLine(velocity);
            uartWriteIntLine(time);
        }
        else
        {
            uartWriteString("Timeout\r\n");
        }
        _delay_ms(100);
    }
}