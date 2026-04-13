// Author: Group 3, Monday 11-2
// Members: Ben Bohan, Lucius Casertano, Vasu Kedia, Micah Case
// Date: 3/23/26
// Assignment: Lab 4
//----------------------------------------------------------------------//

#include "timer.h"

// initTimer0() -- Initialize timer 0 in CTC mode
void initTimer0(){
    // CTC Mode (010)
    TCCR0A &= ~(1 << WGM00);    // WGM00 = 0
    TCCR0A |=  (1 << WGM01);    // WGM01 = 1
    TCCR0B &= ~(1 << WGM02);    // WGM02 = 0

    // Prescaling 64, (011)
    TCCR0B |=  (1 << CS00);      // CS00 = 1
    TCCR0B |=  (1 << CS01);      // CS01 = 1
    TCCR0B &= ~(1 << CS02);      // CS02 = 0

    // Calculations: (16 MHz / 64) = 250 kHz ⇒ 1 tick = 4 µs, so 
    // 1 ms = 250 ticks ⇒ OCR0A = 250 − 1 = 249
    OCR0A = 249;
}

// delayMs(delay) -- Delays the program by unsigned int delay. (1ms)
void delayMs(unsigned int delay){
    unsigned int delayCnt = 0;
    
    TCNT0 = 0;             // Reset Timer0 counter to 0
    TIFR0 |= (1 << OCF0A); // Clear compare flag

    while (delayCnt < delay) {
        // If flagged, 1 ms has elapsed
        if (TIFR0 & (1 << OCF0A)) {
            delayCnt++;            // +1 ms
            TIFR0 |= (1 << OCF0A); // Clear Timer0 compare flag
        }
    }
}

// initTimer1() -- Initialize Timer1 in CTC mode for 1-second timing
void initTimer1(){
    // CTC Mode (0100)
    TCCR1A &= ~(1 << WGM10);    // WGM10 = 0
    TCCR1A &= ~(1 << WGM11);    // WGM11 = 0
    TCCR1B |=  (1 << WGM12);    // WGM12 = 1
    TCCR1B &= ~(1 << WGM13);    // WGM13 = 0

    // Prescaling 1024 (101)
    TCCR1B |=  (1 << CS10);     // CS10 = 1
    TCCR1B &= ~(1 << CS11);     // CS11 = 0
    TCCR1B |=  (1 << CS12);     // CS12 = 1

    // Calculations:
    // (16 MHz / 1024) = 15625 Hz
    // 1 second = 15625 ticks
    // OCR1A = 15625 - 1 = 15624
    OCR1A = 15624;
}

// delaySec(delay) -- Delays the program by "delay" seconds
void delaySec(unsigned int delay){
    unsigned int delayCnt = 0;

    TCNT1 = 0;             // Reset Timer1 counter to 0
    TIFR1 |= (1 << OCF1A); // Clear Timer1 compare match flag

    while (delayCnt < delay) {
        // If flagged, 1 second has elapsed
        if (TIFR1 & (1 << OCF1A)) {
            delayCnt++;            // +1 second
            TIFR1 |= (1 << OCF1A); // Clear Timer1 compare match flag
        }
    }
}