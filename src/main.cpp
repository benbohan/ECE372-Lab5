// Author: Group 3, Monday 11-2
// Members: Ben Bohan, Lucius Casertano, Vasu Kedia, Micah Case
// Date: 3/23/26
// Assignment: Lab 4
//
// Description: This file contains the main state machine for Lab 4.
// The potentiometer controls the dc motor speed/direction using ADC,
// PWM, and the L293D driver. A debounced external interrupt switch
// turns the motor off, and the seven segment display counts down from
// 9 to 0 before the motor turns back on.
//
// Process:
// 1. The motor follows the potentiometer.
// 2. The button is pressed.
// 3. The motor stops.
// 4. The seven segment display counts down from 9 to 0.
// 5. The motor stays off during the countdown.
// 6. When the countdown ends, the motor goes back to following the potentiometer.
//----------------------------------------------------------------------//

#include <avr/io.h>
#include <avr/interrupt.h>

#include "adc.h"
#include "pwm.h"
#include "sevenseg.h"
#include "switch.h"
#include "timer.h"

typedef enum {
    MOTOR_RUNNING,            // Main state: motor follows potentiometer
    BUTTON_DEBOUNCE_PRESS,    // Verify button press
    COUNTDOWN_ACTIVE,         // Disable interrupt & motor during countdown. Reset everything.
} MainState;

// Initial State
volatile MainState mainState = MOTOR_RUNNING;

// Flags updated by ISR / main logic
volatile unsigned char switchPressed = 0;
volatile unsigned char countdownDigit = 9;

void initMotorDirectionPins(void){
    DDRH |= (1 << DDH4) | (1 << DDH5);   // D7 and D8 as outputs
    PORTH &= ~((1 << PH4) | (1 << PH5)); // start low
}

void motorClockwise(void){
    PORTH |= (1 << PH4);
    PORTH &= ~(1 << PH5);
}

void motorCounterClockwise(void){
    PORTH |= (1 << PH5);
    PORTH &= ~(1 << PH4);
}

void motorStop(void){
    PORTH &= ~((1 << PH4) | (1 << PH5));
    motorPWM_Off();
}


int main(void) {
  initTimer0();
  initTimer1();
  initADC();
  initPWM_Pins();
  initSevenSegment();
  initSwitchPD0();
  initMotorDirectionPins();

  sei();
  sevenSegmentOff();
  unsigned int adcValue = 0;
  unsigned int pwmValue = 0;

  //Implement state machine 
	while (1) {

    switch (mainState) {
      // Case 1: Motor running
      case MOTOR_RUNNING:
        adcValue = readADC(); 
        if (adcValue < 512) {
          motorClockwise();
          pwmValue = (511 - adcValue) * 2;
          changeDutyCycle(pwmValue);
        } else if (adcValue > 512) {
          motorCounterClockwise();
          pwmValue = (adcValue - 512) * 2;
          changeDutyCycle(pwmValue);
        } 

        else {
          motorStop();
        }

        // Wait for button press, ISR for debounce
        if (switchPressed) {     
          switchPressed = 0; // reset flag  
          mainState = BUTTON_DEBOUNCE_PRESS;
        }
      break;

      // Case 2: Button released
      case BUTTON_DEBOUNCE_PRESS:
        delayMs(5);
        
        // If still pressed after 5ms, start COUNTDOWN_ACTIVE
        if ((PIND & (1 << PD0)) == 0) {
          motorStop();            // motor off
          disableSwitchInterrupt();  // disable interrupt
          
          countdownDigit = 9;
          displayDigit(countdownDigit);

          mainState = COUNTDOWN_ACTIVE;
        }

        // Debounce detected: go back to MOTOR_RUNNING
        else {
          mainState = MOTOR_RUNNING;
        }
        break;

      // Case 3: Counting down, then Restart motor, enable interrupt turn off 7seg
      case COUNTDOWN_ACTIVE:
        delaySec(1);

        // Countdown 9 --> 0
        if (countdownDigit > 0) {
            countdownDigit--;
            displayDigit(countdownDigit);
        }
        
        // After Countdown, reset for MOTOR_RUNNING
        else {
          sevenSegmentOff();            // 7seg off
          EIFR |= (1 << INTF0);         // clear pending INT0 flag
          enableSwitchInterrupt();      // enable interrupt
          switchPressed = 0;            // reset switch
          mainState = MOTOR_RUNNING;    // reset state
        }
      break;

      // Case Default
      default:
        mainState = MOTOR_RUNNING;
        break;
    }
  }
  return 0;
}

// ISR() -- External interrupt for the push button on PORTD0. Keep ISR short: only set a flag / change state.
ISR(INT0_vect){
    if (mainState == MOTOR_RUNNING) {
        switchPressed = 1;
    }
}