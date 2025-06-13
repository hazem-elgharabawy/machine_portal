#ifndef FIXED_H
#define FIXED_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

// Fixed-point configuration
#define INT_WIDTH 4
#define FRACT_WIDTH 12
#define DATA_WIDTH (INT_WIDTH + FRACT_WIDTH)    
#define PRODUCT_DATA_WIDTH (2*DATA_WIDTH)


#if DATA_WIDTH == 16
typedef int16_t fixed_point_t;
typedef int32_t fixed_point_product_t;
#elif DATA_WIDTH == 18 || DATA_WIDTH == 32
// Fixed-point data type
typedef int32_t fixed_point_t;
typedef int64_t fixed_point_product_t;
#endif
// Converts a fixed-point number to a double
inline double fixed_to_double(fixed_point_product_t value) {
    return (double)value / (1 << FRACT_WIDTH);  // Divide by 2^FRACT_WIDTH
}


// Multiplies two fixed-point numbers
inline fixed_point_t fixed_mul(fixed_point_t a, fixed_point_t b) {
    fixed_point_product_t product = (fixed_point_product_t)a * (fixed_point_product_t)b;  // Perform multiplication
    product = product >> FRACT_WIDTH;  // Adjust for fractional width
    return (fixed_point_t)product;
}

// Converts a double to a fixed-point number
inline fixed_point_t double_to_fixed( double value) {    
    return (fixed_point_t)(value * (1 << FRACT_WIDTH) + 0.5);  // Multiply by 2^FRACT_WIDTH and round
}

inline fixed_point_t fixed_mac(fixed_point_t a, fixed_point_t b, fixed_point_t c, fixed_point_t d){
    fixed_point_product_t product = ((fixed_point_product_t)a * (fixed_point_product_t)b) + ((fixed_point_product_t)c * (fixed_point_product_t)d);
    product = product >> FRACT_WIDTH;
    return (fixed_point_t)product;
}

  
#endif // FIXED_H