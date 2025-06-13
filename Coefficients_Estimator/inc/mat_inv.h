#ifndef MAT_INV_H
#define MAT_INV_H

#include <stdint.h>
#include <stdlib.h>
#include "fixed.h"

// Fixed-point format definitions
#define Q12_FRACTIONAL_BITS 12
#define Q12_SCALE_FACTOR (1 << Q12_FRACTIONAL_BITS)  // Q4.12 format
#define Q12_MAX_VALUE 32767
#define Q12_MIN_VALUE -32768
#define Q12_EPSILON 16  // Small value for numerical stability

// Matrix dimensions
#define MATRIX_SIZE 3
#define SYMMETRIC_MATRIX_ELEMENTS 6
#define FULL_MATRIX_ELEMENTS 9

// Main function declaration
int8_t invert3x3_q412(const fixed_point_t* mat6, fixed_point_t* out9);

#endif // MAT_INV_H
