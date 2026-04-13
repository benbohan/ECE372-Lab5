// Author: Group 3, Monday 11-2
// Members: Ben Bohan, Lucius Casertano, Vasu Kedia, Micah Case
// Date: 4/13/26
// Assignment: Lab 5
//----------------------------------------------------------------------//

#include "timer.h"

// initTimer1() -- Initialize Timer1 in CTC mode for 1 millisecond timing
void initTimer1(){
    // CTC Mode (0100)
    TCCR1A &= ~(1 << WGM10);    // WGM10 = 0
    TCCR1A &= ~(1 << WGM11);    // WGM11 = 0
    TCCR1B |=  (1 << WGM12);    // WGM12 = 1
    TCCR1B &= ~(1 << WGM13);    // WGM13 = 0

    // Prescaling 64 (011)
    TCCR1B |=  (1 << CS10);     // CS10 = 1
    TCCR1B |=  (1 << CS11);     // CS11 = 1
    TCCR1B &= ~(1 << CS12);     // CS12 = 0

    // Calculations:
    // (16 MHz / 64) = 250 kHz
    // 1 ms = 250 ticks
    // OCR1A = 250 - 1 = 249
    OCR1A = 249;
}

// delayMs(delay) -- Delays the program by unsigned int delay. (1ms)
void delayMs(unsigned int delay){
    unsigned int delayCnt = 0;
    
    TCNT1 = 0;             // Reset Timer1 counter to 0
    TIFR1 |= (1 << OCF1A); // Clear compare flag

    while (delayCnt < delay) {
        // If flagged, 1 ms has elapsed
        if (TIFR1 & (1 << OCF1A)) {
            delayCnt++;            // +1 ms
            TIFR1 |= (1 << OCF1A); // Clear Timer1 compare flag
        }
    }
}