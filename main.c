/*
 * main.c for ultrasonic anemometer
 *
 * Created: 22.11.2021 13:52
 * Author : tjark
 * Target : Anemometer.sch
 */

#ifndef __AVR_ATtiny2313A__
#define __AVR_ATtiny2313A__
#endif

#define F_CPU 8000000UL
#define S_SENSOR 100        //Distance betwen Sensors in mm

#define ECHO (PIND & 0x02)
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

    while(1)
    {
        long time = getSensorTime();
        if(time > 0)
        {
            //v = d/t
            long velocity = S_SENSOR/(time*1000);
            uartWriteIntLine(velocity);
        }
        else
        {
            uartWriteString("Timeout\r\n");
        }
    }
}