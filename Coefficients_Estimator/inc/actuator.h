#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdint.h>
#include "fixed.h"

// Optimized actuator structure with packed alignment
typedef struct {
    fixed_point_t in_r, in_i;               // Input values
    fixed_point_t out_r, out_i;             // Output values
} Actuator_S;

// Pre-computed coefficients for better performance
#if DATA_WIDTH == 16
    #define A10_R 0x0FFC
    #define A10_I 0x0031
    #define A30_R 0x001B
    #define A30_I 0x0008
    #define A50_R 0x008B
    #define A50_I 0x0121
#elif DATA_WIDTH == 18 || DATA_WIDTH == 32
    #define A10_R 0x03FC
    #define A10_I 0x0031
    #define A30_R 0x001B
    #define A30_I 0x0008
    #define A50_R 0x008B
    #define A50_I 0x0121
#endif

// Function prototypes
void actuator_func(Actuator_S *act);

#endif // ACTUATOR_H