/*
 * main.c for ultrasonic anemometer
 *
 * Created: 22.11.2021 13:52
 * Author : tjark
 * Target : //TODO
 */

#ifndef __AVR_ATtiny2313__
#define __AVR_ATtiny2313__
#endif

#define F_CPU 8000000UL
#define S_SENSOR 100        //Distance betwen Sensors in mm

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "HC_SR04.h"

int main(void)
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
        uartWriteString("Timeout");
    }

}