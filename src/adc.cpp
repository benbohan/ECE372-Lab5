#include <avr/io.h>
#include "adc.h"

void initADC(void) {
    DDRF &= ~(1 << DDF0); // Set PF0 as input for ADC
    PORTF &= ~(1 << PORTF0); // Disable pull-up resistor on PF0

    ADMUX |= (1 << REFS0); // AVcc reference voltage

    // Set ADC prescaler to 128 for 125kHz ADC clock with 16MHz clock
    ADCSRA |= (1 << ADEN); // Enable ADC
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t readADC(void) {
    uint8_t low, high;

    ADCSRA |= (1 << ADSC); // Start ADC conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
    
    low = ADCL; // Read low byte first
    high = ADCH; // Read high byte

    return ((uint16_t)high << 8) | low; // Combine high and low bytes
}