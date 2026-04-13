// Author: Group 3, Monday 11-2
// Members: Ben Bohan, Lucius Casertano, Vasu Kedia, Micah Case
// Date: 4/13/26
// Assignment: Lab 5
// Description: This file contains function prototypes to be implemented in
// timer.cpp and to be included in main.cpp.
//----------------------------------------------------------------------//

#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

// Timer1: millisecond timer for switch debounce and general delays
void initTimer1();
void delayMs(unsigned int delay);

#endif
