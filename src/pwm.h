#ifndef pwm_H
#define pwm_H

#include <stdint.h>

void initPWM_Pins(void);
void changeDutyCycle(uint16_t adcValue);
void motorPWM_Off(void);

#endif