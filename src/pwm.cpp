#include <avr/io.h>
#include "pwm.h"

void initPWM_Pins(void) {
    DDRE |= (1 << PE3); // Set PE3 as output for PWM

    // Set Fast PWM mode with ICR3 as TOP
    TCCR3A |= (1 << WGM31);
    TCCR3B |= (1 << WGM32) | (1 << WGM33);

    TCCR3A |= (1 << COM3A1); // Clear OC3A on compare match, set at BOTTOM
    TCCR3B |= (1 << CS31);   // Prescaler of 8

    ICR3 = 1023; // Set TOP value for 10-bit resolution
    OCR3A = 0;   // Start with motor off
}

void changeDutyCycle(uint16_t adcValue) {
    if (adcValue > 1023) {
        adcValue = 1023; // Cap at maximum value
    }

    OCR3A = adcValue; // Set duty cycle based on ADC value
}

void motorPWM_Off(void) {
    OCR3A = 0; // Set duty cycle to 0 to turn off motor
}

