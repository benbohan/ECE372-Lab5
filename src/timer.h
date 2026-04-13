// Author: Group 3, Monday 11-2
// Members: Ben Bohan, Lucius Casertano, Vasu Kedia, Micah Case
// Date: 3/23/26
// Assignment: Lab 4
// Description: This file contains function prototypes to be implemented in
// timer.cpp and to be included in main.cpp.
//----------------------------------------------------------------------//

#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

// Timer0: switch debounce
void initTimer0();
void delayMs(unsigned int delay);

// Timer1: 10 second countdown for 7-segment display
void initTimer1();
void delaySec(unsigned int delay);

#endif
