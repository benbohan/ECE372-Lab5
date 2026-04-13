// Author: Group 3, Monday 11-2
// Members: Ben Bohan, Lucius Casertano, Vasu Kedia, Micah Case
// Date: 4/13/26
// Assignment: Lab 5
//----------------------------------------------------------------------//

#include "pwm.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// OC3A on Mega 2560 = PE3 = digital pin 5
#define BUZZER_DDR   DDRE
#define BUZZER_PORT  PORTE
#define BUZZER_BIT   PE3

// Frequency sweep range for chirp
#define CHIRP_MIN_FREQ  1200
#define CHIRP_MAX_FREQ  2600
#define CHIRP_STEP      100

void initPWM(void) {
    // Set buzzer pin as output
    BUZZER_DDR |= (1 << BUZZER_BIT);
    BUZZER_PORT &= ~(1 << BUZZER_BIT);

    // Clear Timer3 control registers
    TCCR3A = 0;
    TCCR3B = 0;
    TCNT3 = 0;

    // Fast PWM mode 14:
    // WGM33:WGM30 = 1110
    // TOP = ICR3
    TCCR3A |= (1 << WGM31);
    TCCR3B |= (1 << WGM32) | (1 << WGM33);

    // Don't start timer or connect OC3A yet
    setPWMFrequency(CHIRP_MIN_FREQ);
}

void setPWMFrequency(uint16_t frequency) {
    uint32_t top;

    // Keep frequency in a reasonable range
    if (frequency < 100) {
        frequency = 100;
    }
    if (frequency > 10000) {
        frequency = 10000;
    }

    // Prescaler = 8
    // Fast PWM frequency:
    // f = F_CPU / (N * (1 + TOP))
    top = (F_CPU / (8UL * (uint32_t)frequency)) - 1;

    if (top > 65535UL) {
        top = 65535UL;
    }

    ICR3 = (uint16_t)top;

    // 50% duty cycle
    OCR3A = ICR3 / 2;
}

void startPWM(void) {
    // Non-inverting mode on OC3A
    TCCR3A |= (1 << COM3A1);
    TCCR3A &= ~(1 << COM3A0);

    // Start Timer3 with prescaler = 8
    TCCR3B &= ~((1 << CS32) | (1 << CS31) | (1 << CS30));
    TCCR3B |= (1 << CS31);
}

void stopPWM(void) {
    // Disconnect OC3A from timer
    TCCR3A &= ~((1 << COM3A1) | (1 << COM3A0));

    // Stop timer clock
    TCCR3B &= ~((1 << CS32) | (1 << CS31) | (1 << CS30));

    // Drive pin low
    BUZZER_PORT &= ~(1 << BUZZER_BIT);
}

void chirpPWM(void) {
    static uint16_t frequency = CHIRP_MIN_FREQ;
    static int direction = 1;

    setPWMFrequency(frequency);
    startPWM();

    // If we're increasing frequency, add step. If we hit max, reverse direction.
    if (direction > 0) {
        frequency += CHIRP_STEP;
        if (frequency >= CHIRP_MAX_FREQ) {
            frequency = CHIRP_MAX_FREQ;
            direction = -1;
        }
    } else {
        frequency -= CHIRP_STEP;
        if (frequency <= CHIRP_MIN_FREQ) {
            frequency = CHIRP_MIN_FREQ;
            direction = 1;
        }
    }
}