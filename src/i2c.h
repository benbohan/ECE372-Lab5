#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stdint.h>

/*
 * MPU-6050 address and register definitions
 * AD0 tied low -> 7-bit slave address = 0x68
 */
#define MPU_ADDR      0x68

#define ACCEL_XOUT_H  0x3B
#define ACCEL_XOUT_L  0x3C
#define ACCEL_YOUT_H  0x3D
#define ACCEL_YOUT_L  0x3E
#define ACCEL_ZOUT_H  0x3F
#define ACCEL_ZOUT_L  0x40

#define PWR_MGMT_1    0x6B
#define WHO_AM_I      0x75

void InitI2C(void);
void StartI2C_Trans(unsigned char SLA);
void StopI2C_Trans(void);
void Write(unsigned char data);
unsigned char Read_from(unsigned char SLA, unsigned char MEMADDRESS);
unsigned char Read_data(void);

#endif