#include "spi_matrix.h"
#include "config.h"
#include <avr/io.h>

// Simple 8x8 bitmaps for faces
static const uint8_t SMILE_FACE[8] = {
    0b00111100,
    0b01000010,
    0b10100101,
    0b10000001,
    0b10100101,
    0b10011001,
    0b01000010,
    0b00111100
};

static const uint8_t FROWN_FACE[8] = {
    0b00111100,
    0b01000010,
    0b10100101,
    0b10000001,
    0b10011001,
    0b10100101,
    0b01000010,
    0b00111100
};

static inline void csLow() {
    PORTB &= ~(1 << PB0); // D53 low
}

static inline void csHigh() {
    PORTB |= (1 << PB0); // D53 high
}

static uint8_t spiTransfer(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF))) {
    }
    return SPDR;
}

static void maxWrite(uint8_t reg, uint8_t data) {
    csLow();
    spiTransfer(reg);
    spiTransfer(data);
    csHigh();
}

static void drawPattern(const uint8_t pattern[8]) {
    for (uint8_t row = 0; row < 8; row++) {
        maxWrite(row + 1, pattern[row]);
    }
}

void initSPI_Matrix() {
    // Mega hardware SPI pins:
    // D53 = PB0 = SS/CS
    // D52 = PB1 = SCK
    // D51 = PB2 = MOSI
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);

    csHigh();

    // SPI master, enable, Fosc/16, mode 0
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
    SPSR = 0;

    // MAX7219 setup
    maxWrite(0x0F, 0x00); // display test off
    maxWrite(0x09, 0x00); // no decode mode
    maxWrite(0x0A, 0x03); // low-medium intensity
    maxWrite(0x0B, 0x07); // scan limit = all 8 rows
    maxWrite(0x0C, 0x01); // normal operation (exit shutdown)

    clearMatrix();
}

void clearMatrix() {
    for (uint8_t row = 1; row <= 8; row++) {
        maxWrite(row, 0x00);
    }
}

void setFace(FaceState face) {
    if (face == FACE_FROWN) {
        drawPattern(FROWN_FACE);
    } else {
        drawPattern(SMILE_FACE);
    }
}

