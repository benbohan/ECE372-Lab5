#include <avr/io.h>
#include <stdint.h>
#include "sevenseg.h"

/*
WIRING:
D22-> pin14 (serial)
D23-> pin12 (latch)
D24-> pin11 (clock)

bit 0 -> segment a (QA)
bit 1 -> segment b (QB)
bit 2 -> segment c (QC)
bit 3 -> segment d (QD)
bit 4 -> segment e (QE)
bit 5 -> segment f (QF)
bit 6 -> segment g (QG)
bit 7 -> decimal point (QH)
*/

#define SHIFTREG_DDR   DDRA
#define SHIFTREG_PORT  PORTA

#define SER_BIT        PA0   // D22
#define RCLK_BIT       PA1   // D23
#define SRCLK_BIT      PA2   // D24

static const uint8_t digitPatterns[10] = {
    0x3F,  // 0 -> a b c d e f
    0x06,  // 1 -> b c
    0x5B,  // 2 -> a b d e g
    0x4F,  // 3 -> a b c d g
    0x66,  // 4 -> b c f g
    0x6D,  // 5 -> a c d f g
    0x7D,  // 6 -> a c d e f g
    0x07,  // 7 -> a b c
    0x7F,  // 8 -> a b c d e f g
    0x6F   // 9 -> a b c d f g
};

// Pulses high then low on the specified bit number of the shift register port
static void pulsePin(uint8_t bitNumber) {
    SHIFTREG_PORT |= (1 << bitNumber);
    SHIFTREG_PORT &= ~(1 << bitNumber);
}

static void shiftRegisterWrite(uint8_t value) {
    int8_t i;

    // Start with latch low so we don't update the outputs untildone shifting
    SHIFTREG_PORT &= ~(1 << RCLK_BIT);

    for (i = 7; i >= 0; i--) {
        if (value & (1 << i)) {
            SHIFTREG_PORT |= (1 << SER_BIT);
        }
        else {
            SHIFTREG_PORT &= ~(1 << SER_BIT);
        }

        pulsePin(SRCLK_BIT); // Clock 1 bit into the shift register
    }

    // Move shifted data to the output storage register
    pulsePin(RCLK_BIT);
}

// Initialize the 7-segment display (shift register) pins and turn off the display
void initSevenSegment(void) {
    SHIFTREG_DDR |= (1 << SER_BIT);
    SHIFTREG_DDR |= (1 << RCLK_BIT);
    SHIFTREG_DDR |= (1 << SRCLK_BIT);

    SHIFTREG_PORT &= ~(1 << SER_BIT);
    SHIFTREG_PORT &= ~(1 << RCLK_BIT);
    SHIFTREG_PORT &= ~(1 << SRCLK_BIT);

    sevenSegmentOff();
}

// Can send any pattern to the 7-segment display
void sevenSegmentDisplay(uint8_t pattern) {
    shiftRegisterWrite(pattern);
}

// DIsplay the specified digit (0-9) on the 7-segment display
void displayDigit(uint8_t digit) {
    if (digit < 10) {
        sevenSegmentDisplay(digitPatterns[digit]);
    }
    else {
        sevenSegmentOff(); // Invalid digit so turn off display
    }
}

void sevenSegmentOff(void) {
    shiftRegisterWrite(0x00); // All segments off
}

