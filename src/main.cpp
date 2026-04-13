// Author: Group 3, Monday 11-2
// Members: Ben Bohan, Lucius Casertano, Vasu Kedia, Micah Case
// Date: 4/13/26
// Assignment: Lab 5
//
// Description: This file contains the main state machine for Lab 5.
// The MPU6050 accelerometer is read over I2C. The 8x8 LED matrix is
// controlled over SPI. When the measured tilt exceeds the threshold,
// the display changes from a smiley face to a frowny face and the piezo
// buzzer chirps. A debounced external interrupt switch silences the alarm.
//----------------------------------------------------------------------//

#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#include "config.h"
#include "i2c.h"
#include "pwm.h"
#include "spi_matrix.h"
#include "switch.h"
#include "timer.h"

// Main display / alarm state machine
typedef enum {
    DISPLAY_SMILE,          // safe tilt, smiley face, buzzer off
    DISPLAY_FROWN,          // tilt exceeded, frowny face, buzzer on
    ALARM_SILENCED          // tilt exceeded, alarm silenced by button
} MainState;

// Switch debounce state machine
typedef enum {
    SWITCH_WAIT,            // waiting for interrupt flag
    SWITCH_DEBOUNCE_PRESS,  // verify button press
    SWITCH_WAIT_RELEASE,    // wait until button is released
    SWITCH_DEBOUNCE_RELEASE // verify release before returning to WAIT
} SwitchState;

// Initial states
volatile MainState mainState = DISPLAY_SMILE;
volatile SwitchState switchState = SWITCH_WAIT;

// Flags updated by ISR / main logic
volatile unsigned char switchFlag = 0;
volatile unsigned char alarmLatched = 0;

// Accelerometer data
int xAccel = 0;
int yAccel = 0;
int zAccel = 0;

// High / low bytes for accelerometer registers
unsigned char xHigh = 0;
unsigned char xLow  = 0;
unsigned char yHigh = 0;
unsigned char yLow  = 0;
unsigned char zHigh = 0;
unsigned char zLow  = 0;

int main(void) {
    init();
    Serial.begin(9600);

    initTimer1();
    initSwitchINT4();
    enableSwitchInterrupt();

    InitI2C();
    initSPI_Matrix();
    initPWM();

    sei();

    // Start MPU6050
    StartI2C_Trans(MPU6050_SLA);
    Write(PWR_MGMT_1);
    Write(0x00);
    StopI2C_Trans();

    setFace(FACE_SMILE);
    motorPWM_Off();

    while (1) {
        // ------------------------------------------------------------
        // Read accelerometer registers
        // ------------------------------------------------------------
        xHigh = Read_from(MPU6050_SLA, ACCEL_XOUT_H);
        xLow  = Read_from(MPU6050_SLA, ACCEL_XOUT_L);
        yHigh = Read_from(MPU6050_SLA, ACCEL_YOUT_H);
        yLow  = Read_from(MPU6050_SLA, ACCEL_YOUT_L);
        zHigh = Read_from(MPU6050_SLA, ACCEL_ZOUT_H);
        zLow  = Read_from(MPU6050_SLA, ACCEL_ZOUT_L);

        xAccel = ((int)xHigh << 8) | xLow;
        yAccel = ((int)yHigh << 8) | yLow;
        zAccel = ((int)zHigh << 8) | zLow;

        Serial.print("X: ");
        Serial.print(xAccel);
        Serial.print("  Y: ");
        Serial.print(yAccel);
        Serial.print("  Z: ");
        Serial.println(zAccel);

        // ------------------------------------------------------------
        // Main display / alarm state machine
        // ------------------------------------------------------------
        switch (mainState) {
            case DISPLAY_SMILE:
                // TODO: display smiley face on LED matrix
                // TODO: make sure buzzer is off

                // If threshold exceeded, latch alarm and go to frown
                if ((yAccel > Y_THRESHOLD) || (yAccel < -Y_THRESHOLD) ||
                    (zAccel > Z_THRESHOLD) || (zAccel < -Z_THRESHOLD)) {
                    alarmLatched = 1;
                    mainState = DISPLAY_FROWN;
                }
                break;

            case DISPLAY_FROWN:
                // TODO: display frowny face on LED matrix
                // TODO: chirp buzzer using PWM

                // Stay in alarm state until switch silences it
                if (alarmLatched == 0) {
                    mainState = ALARM_SILENCED;
                }
                break;

            case ALARM_SILENCED:
                // TODO: keep buzzer off
                // TODO: decide whether face stays frown or returns to smile
                // Usually:
                // - if still tilted, keep frown but no sound
                // - if returned to safe position, go back to smile

                if ((yAccel < Y_THRESHOLD) && (yAccel > -Y_THRESHOLD) &&
                    (zAccel < Z_THRESHOLD) && (zAccel > -Z_THRESHOLD)) {
                    mainState = DISPLAY_SMILE;
                }

                // Optional:
                // if you want a new tilt event to retrigger the alarm,
                // add that logic here
                break;

            default:
                mainState = DISPLAY_SMILE;
                break;
        }

        // ------------------------------------------------------------
        // Switch debounce state machine
        // ------------------------------------------------------------
        switch (switchState) {
            case SWITCH_WAIT:
                if (switchFlag) {
                    switchFlag = 0;
                    switchState = SWITCH_DEBOUNCE_PRESS;
                }
                break;

            case SWITCH_DEBOUNCE_PRESS:
                delayMs(DEBOUNCE_TIME);

                // TODO: verify button is still pressed
                // Since pull-up is used, pressed usually reads low
                if ((PIND & (1 << PD0)) == 0) {
                    alarmLatched = 0;
                    // TODO: turn buzzer off here if needed
                    disableSwitchInterrupt();
                    switchState = SWITCH_WAIT_RELEASE;
                }
                else {
                    switchState = SWITCH_WAIT;
                }
                break;

            case SWITCH_WAIT_RELEASE:
                // Wait for release
                if (PIND & (1 << PD0)) {
                    switchState = SWITCH_DEBOUNCE_RELEASE;
                }
                break;

            case SWITCH_DEBOUNCE_RELEASE:
                delayMs(DEBOUNCE_TIME);

                if (PIND & (1 << PD0)) {
                    EIFR |= (1 << INTF0);     // clear pending INT0 flag
                    enableSwitchInterrupt();  // re-enable interrupt
                    switchState = SWITCH_WAIT;
                }
                else {
                    switchState = SWITCH_WAIT_RELEASE;
                }
                break;

            default:
                switchState = SWITCH_WAIT;
                break;
        }
    }
    return 0;
}

// ISR() -- External interrupt for the push button on PORTD0.
ISR(INT0_vect) {
    switchFlag = 1;
}