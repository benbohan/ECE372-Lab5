// Author: Group 3, Monday 11-2
// Members: Ben Bohan, Lucius Casertano, Vasu Kedia, Micah Case
// Date: 4/13/26
// Assignment: Lab 5
//----------------------------------------------------------------------//

#include "i2c.h"

void InitI2C(void) {
    // Prescaler = 1, TWPS1 = 0, TWPS0 = 0
    TWSR &= ~((1 << TWPS1) | (1 << TWPS0));

    // Bit rate standard
    TWBR = 72;

    // TWI enable
    TWCR = (1 << TWEN);
}

void StartI2C_Trans(unsigned char SLA) {
    // start condition + enable TWI
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // wait for start to finish
    while ((TWCR & (1 << TWINT)) == 0) {
    }

    // load SLA + write bit (0)
    TWDR = (SLA << 1) | 0;

    // address transmission
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait for address transmission
    while ((TWCR & (1 << TWINT)) == 0) {
    }
}

void StopI2C_Trans(void) {
    // Send stop condition
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void Write(unsigned char data) {
    // load data
    TWDR = data;

    // start transmission
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait for transmission to finish
    while ((TWCR & (1 << TWINT)) == 0) {
    }
}

unsigned char Read_data(void) {
    // recieve data + send NACK (since we only want to read one byte)
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait for recieve to finish
    while ((TWCR & (1 << TWINT)) == 0) {
    }

    return TWDR;
}

unsigned char Read_from(unsigned char SLA, unsigned char MEMADDRESS) {
    unsigned char data;

    // write memory address to slave
    StartI2C_Trans(SLA);
    Write(MEMADDRESS);

    // repeated start + SLA + read bit
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // wait for repeated start to finish
    while ((TWCR & (1 << TWINT)) == 0) {
    }

    // load SLA + read bit (1)
    TWDR = (SLA << 1) | 1;

    // trigger address transmission
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait for address transmission to finish
    while ((TWCR & (1 << TWINT)) == 0) {
    }

    // read data from slave
    data = Read_data();

    // end transmission
    StopI2C_Trans();

    return data;
}