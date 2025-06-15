#ifndef MAT_INV_H
#define MAT_INV_H

#include <stdint.h>
#include <stdlib.h>
#include "fixed.h"


// Optimized actuator structure with packed alignment
typedef struct {
    fixed_point_t a11, a12, a13;               // Input values
    fixed_point_t a22, a23;             
    fixed_point_t a33;              // Coefficients
} Matrix_S;

// Main function declaration
int8_t mat_inv(const Matrix_S* mat, Matrix_S* inv);

#endif // MAT_INV_H
