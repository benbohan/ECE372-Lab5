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

    // Wake up MPU6050
    StartI2C_Trans(MPU6050_ADDR);
    Write(REG_PWR_MGMT_1);
    Write(0x00);
    StopI2C_Trans();

    setFace(FACE_SMILE);
    stopPWM();

    while (1) {
        // Read accelerometer registers
        xHigh = Read_from(MPU6050_ADDR, REG_ACCEL_XOUT_H);
        xLow  = Read_from(MPU6050_ADDR, REG_ACCEL_XOUT_L);
        yHigh = Read_from(MPU6050_ADDR, REG_ACCEL_YOUT_H);
        yLow  = Read_from(MPU6050_ADDR, REG_ACCEL_YOUT_L);
        zHigh = Read_from(MPU6050_ADDR, REG_ACCEL_ZOUT_H);
        zLow  = Read_from(MPU6050_ADDR, REG_ACCEL_ZOUT_L);

        xAccel = (int16_t)(((uint16_t)xHigh << 8) | xLow);
        yAccel = (int16_t)(((uint16_t)yHigh << 8) | yLow);
        zAccel = (int16_t)(((uint16_t)zHigh << 8) | zLow);

        Serial.print("X: ");
        Serial.print(xAccel);
        Serial.print("  Y: ");
        Serial.print(yAccel);
        Serial.print("  Z: ");
        Serial.println(zAccel);

        // Main display / alarm state machine
        switch (mainState) {
            case DISPLAY_SMILE:
                setFace(FACE_SMILE);
                stopPWM();

                // If threshold exceeded, latch alarm and go to frown
                if ((yAccel > Y_DELTA_THRESHOLD) || (yAccel < -Y_DELTA_THRESHOLD) ||
                    (zAccel > Z_DELTA_THRESHOLD) || (zAccel < -Z_DELTA_THRESHOLD)) {
                    alarmLatched = 1;
                    mainState = DISPLAY_FROWN;
                }
                break;

            case DISPLAY_FROWN:
                setFace(FACE_FROWN);
                chirpPWM();

                // Stay in alarm state until switch silences it
                if (alarmLatched == 0) {
                    mainState = ALARM_SILENCED;
                }
                break;

            case ALARM_SILENCED:
                stopPWM();

                if ((yAccel < Y_DELTA_THRESHOLD) && (yAccel > -Y_DELTA_THRESHOLD) &&
                    (zAccel < Z_DELTA_THRESHOLD) && (zAccel > -Z_DELTA_THRESHOLD)) {
                    mainState = DISPLAY_SMILE;
                }
                break;

            default:
                mainState = DISPLAY_SMILE;
                break;
        }

        // Switch debounce state machine
        switch (switchState) {
            case SWITCH_WAIT:
                if (switchFlag) {
                    switchFlag = 0;
                    switchState = SWITCH_DEBOUNCE_PRESS;
                }
                break;

            case SWITCH_DEBOUNCE_PRESS:
                delayMs(DEBOUNCE_MS);

                if (switchPressed()) {
                    alarmLatched = 0;
                    stopPWM();
                    disableSwitchInterrupt();
                    switchState = SWITCH_WAIT_RELEASE;
                }
                else {
                    switchState = SWITCH_WAIT;
                }
                break;

            case SWITCH_WAIT_RELEASE:
                // Wait for release
                if (!switchPressed()) {
                    switchState = SWITCH_DEBOUNCE_RELEASE;
                }
                break;

            case SWITCH_DEBOUNCE_RELEASE:
                delayMs(DEBOUNCE_MS);

                if (!switchPressed()) {
                    clearSwitchInterruptFlag();
                    enableSwitchInterrupt();
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

// ISR() -- External interrupt for the push button on INT4.
ISR(INT4_vect) {
    switchFlag = 1;
}