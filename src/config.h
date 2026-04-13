#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// -------------------------------
// Arduino Mega 2560 pin choices
// -------------------------------
// MPU6050 / GY-521 (I2C)
//   SDA -> pin 20
//   SCL -> pin 21
//   AD0 -> GND  => 7-bit address 0x68
//   VCC -> 5V, GND -> GND

// MAX7219 matrix (hardware SPI on Mega)
//   DIN -> pin 51 (MOSI)
//   CLK -> pin 52 (SCK)
//   CS  -> pin 53 (SS, chosen as chip select)
//   VCC -> 5V, GND -> GND

// Pushbutton
//   one side -> pin 22
//   other side -> GND
//   uses INPUT_PULLUP
constexpr uint8_t BUTTON_PIN = 22;

// Passive buzzer
//   + -> pin 5 (OC3A)
//   - -> GND
constexpr uint8_t BUZZER_PIN = 5;

// MAX7219 chip select on hardware SS pin
constexpr uint8_t MATRIX_CS_PIN = 53;

// MPU6050 address (AD0 low)
constexpr uint8_t MPU6050_ADDR = 0x68;

// MPU6050 register addresses
constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B;
constexpr uint8_t REG_ACCEL_YOUT_H = 0x3D;
constexpr uint8_t REG_ACCEL_ZOUT_H = 0x3F;
constexpr uint8_t REG_ACCEL_XOUT_L = 0x3C;
constexpr uint8_t REG_ACCEL_YOUT_L = 0x3E;
constexpr uint8_t REG_ACCEL_ZOUT_L = 0x40;
constexpr uint8_t REG_ACCEL_CONFIG = 0x1C;
constexpr uint8_t REG_PWR_MGMT_1   = 0x6B;
constexpr uint8_t REG_WHO_AM_I     = 0x75;

// Debounce time
constexpr uint16_t DEBOUNCE_MS = 20;

// Initial threshold guesses in raw accelerometer counts.
// Replace these with values found experimentally in your lab.
constexpr int16_t Y_DELTA_THRESHOLD = 6000;
constexpr int16_t Z_DELTA_THRESHOLD = 6000;

#endif
