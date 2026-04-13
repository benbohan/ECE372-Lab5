// Description: This file implements turnOnLED and initLED to control
// the LEDs.
//----------------------------------------------------------------------//


#include <avr/io.h>
#include <util/delay.h>
#include "led.h"

/* Initialize PD0, PD1, PD2, and PD3 to outputs
*/
void initLED(){
    DDRD |= (1 << 0); PORTD |= (1 << 0); // Pin 18 → PD0
    DDRD |= (1 << 1); PORTD |= (1 << 1); // Pin 19 → PD1
    DDRD |= (1 << 2); PORTD |= (1 << 2); // Pin 20 → PD2
    DDRD |= (1 << 3); PORTD |= (1 << 3); // Pin 21 → PD3
}

/* This must be one line of code.
* In this function you will be giving a number "num" which will be represented
* in binary by four LEDs. You must effectively assign the lowest four bits of
* "num" to the appropriate bits of PORTD.
*/
void turnOnLEDWithChar(unsigned char num){
  PORTD = (PORTD & 0xF0) | (num & 0x0F);
}