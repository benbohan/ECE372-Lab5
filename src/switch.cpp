// Author: Group 3, Monday 11-2
// Members: Ben Bohan, Lucius Casertano, Vasu Kedia, Micah Case
// Date: 4/13/26
// Assignment: Lab 5
// Description: This file implements the initialization of an external
// switch.
//----------------------------------------------------------------------//

#include "switch.h"
#include <avr/io.h>

/* initSwitchPD0() -- switch initialization
*  1. Initializes pull-up resistor on PD0
*  2. Sets it into input mode
*  3. Enables external interrupt INT0 configured to logic changes
*/
void initSwitchINT4() {
    DDRE  &= ~(1 << DDE4);     // PE4 input
    PORTE |=  (1 << PORTE4);   // enable pull-up

    // INT4 on any logical change
    EICRB |=  (1 << ISC40);
    EICRB &= ~(1 << ISC41);
}

void enableSwitchInterrupt(){
    EIMSK |= (1 << INT4);      // enable INT0
}

void disableSwitchInterrupt(){
    EIMSK &= ~(1 << INT4);     // disable INT0
}

unsigned char switchPressed() {
    // active low because of pull-up
    return ((PINE & (1 << PINE4)) == 0);
}