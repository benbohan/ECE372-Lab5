#ifndef sevenseg_H
#define sevenseg_H

#include <stdint.h>

void initSevenSegment(void);
void displayDigit(uint8_t digit);
void sevenSegmentOff(void);
void sevenSegmentDisplay(uint8_t pattern);

#endif