#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdint.h>
#include "fixed.h"

// Actuator structure
typedef struct {
    fixed_point_t in_r, in_i;               // Input values
    fixed_point_t out_r, out_i;             // Output values
} Actuator_S;

#if DATA_WIDTH == 16
#define A10_R 0b0000111111111100
#define A10_I 0b0000000000110001

#define A30_R 0b0000000000011011 
#define A30_I 0b0000000000001000

#define A50_R 0b0000000010001011
#define A50_I 0b0000000100100001

#elif DATA_WIDTH == 18 || DATA_WIDTH == 32
#define A10_R 0b000000111111111100
#define A10_I 0b000000000000110001

#define A30_R 0b000000000000011011 
#define A30_I 0b000000000000001000

#define A50_R 0b000000000010001011
#define A50_I 0b000000000100100001

#endif

// Function prototypes
void actuator_func(Actuator_S *act);




#endif // ACTUATOR_H
