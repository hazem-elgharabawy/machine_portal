#ifndef MAT_INV_H
#define MAT_INV_H

#include <stdint.h>
#include <stdlib.h>
#include "fixed.h"


// Matrix_S is a structure representing a symmetric 3x3 matrix using fixed-point arithmetic.
typedef struct {
    fixed_point_t a11, a12, a13;               // Input values
    fixed_point_t a22, a23;             
    fixed_point_t a33;              // Coefficients
} Matrix_S;

//function declaration
int8_t mat_inv(const Matrix_S* mat, Matrix_S* inv);
int8_t mat_inv_parallel(const Matrix_S* mat, Matrix_S* inv);
#endif // MAT_INV_H
