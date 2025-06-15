#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdint.h>
#include "fixed.h"

// Optimized actuator structure with packed alignment
typedef struct {
    fixed_point_t in_r, in_i;               // Input values
    fixed_point_t a10_r, a10_i;             
    fixed_point_t a30_r, a30_i;             // Coefficients
    fixed_point_t a50_r, a50_i;
    fixed_point_t out_r, out_i;             // Output values
} Actuator_S;

/*
// Pre-computed coefficients for better performance
   #define A10_R 0b000000010000010101
   #define A10_I 0b000000000011000010
   #define A30_R 0b111111111100010011
   #define A30_I 0b111111111001100010
   #define A50_R 0b000000000000100110
   #define A50_I 0b000000000110011100
*/
   // Function prototypes
void actuator_func(Actuator_S *act);

#endif // ACTUATOR_H