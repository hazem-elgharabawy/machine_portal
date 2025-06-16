#ifndef MAT_INV_H
#define MAT_INV_H

#include <stdint.h>
#include <stdlib.h>
#include "actuator.h"
#include "fixed.h"
#include "mat_inv.h"

struct Matrix_S;

// Main function declaration
void coeff_update(const Matrix_S* yhy_inv, Matrix_S* yhe_mat, Actuator_S* actuator);

#endif // MAT_INV_H
