/*
 * MotorController.c
 *
 * Created: 04.05.2021 20:41:36
 * Author : tjark
 * Target : Motorcontroller.brd
 */ 
/*
 * Bitmap Corection Word:
 * Bit		Function
 * 0-1		Allways ON
 * 2-11 	corection factor
 * 12-15	level
 */

#ifndef __AVR_ATtiny2313__
#define __AVR_ATtiny2313__
#endif

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "HC_SR04.h"

int main(void)
{

}