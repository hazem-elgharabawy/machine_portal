#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdint.h>
#include "fixed.h"

// Optimized actuator structure with packed alignment
typedef struct {
    fixed_point_t in_r, in_i;               // Input values
    fixed_point_t a10_r, a10_i;             // Coefficients 
    fixed_point_t a30_r, a30_i;             // Coefficients 
    fixed_point_t a50_r, a50_i;             // Coefficients 
    fixed_point_t out_r, out_i;             // Output values
} Actuator_S;

// Pre-computed coefficients used in the pre distortion test

    #define A10_R 0x0FFC
    #define A10_I 0x0031
    #define A30_R 0x001B
    #define A30_I 0x0008
    #define A50_R 0x008B
    #define A50_I 0x0121

    // Function prototypes
void actuator_func(Actuator_S *act);

#endif // ACTUATOR_H