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
void initSwitchPD0(){
    DDRD  &= ~(1 << DDD0);     // set PD0 as input
    PORTD |=  (1 << PORTD0);   // enable pull-up resistor

    EICRA |=  (1 << ISC00);    // any logical change on INT0
    EICRA &= ~(1 << ISC01);

    EIMSK |=  (1 << INT0);     // enable external interrupt INT0
}

void enableSwitchInterrupt(){
    EIMSK |= (1 << INT0);      // enable INT0
}

void disableSwitchInterrupt(){
    EIMSK &= ~(1 << INT0);     // disable INT0
}
