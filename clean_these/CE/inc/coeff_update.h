#ifndef COEFF_UPDATE_H
#define COEFF_UPDATE_H

#include <stdint.h>
#include <stdlib.h>
#include "actuator.h"
#include "fixed.h"
#include "mat_inv.h"

//function declaration
void coeff_update(const Matrix_S* yhy_inv, const Matrix_S* yhe_mat, Actuator_S* actuator);
void coeff_update_parallel(const Matrix_S* yhy_inv, const Matrix_S* yhe_mat, Actuator_S* actuator);

#endif // COEFF_UPDATE_H
