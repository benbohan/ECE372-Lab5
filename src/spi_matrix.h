#ifndef SPI_MATRIX_H
#define SPI_MATRIX_H

#include <Arduino.h>

enum FaceState {
    FACE_SMILE = 0,
    FACE_FROWN = 1
};

void initSPI_Matrix();
void clearMatrix();
void setFace(FaceState face);

#endif
