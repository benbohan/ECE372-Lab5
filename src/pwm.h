#ifndef PWM_H
#define PWM_H

#include <avr/io.h>
#include <stdint.h>

void initPWM(void);
void startPWM(void);
void stopPWM(void);
void setPWMFrequency(uint16_t frequency);
void chirpPWM(void);

#endif