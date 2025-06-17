#ifndef FIXED_H
#define FIXED_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


// Fixed-point configuration
#define INT_WIDTH 8
#define FRACT_WIDTH 10
#define DATA_WIDTH INT_WIDTH + FRACT_WIDTH    
#define PRODUCT_DATA_WIDTH 2*DATA_WIDTH

#if DATA_WIDTH == 16
typedef int16_t fixed_point_t;
typedef int32_t fixed_point_product_t;
#elif DATA_WIDTH == 18 || DATA_WIDTH == 32
// Fixed-point data type
typedef int32_t fixed_point_t;
typedef int64_t fixed_point_product_t;
#endif

// Helper function for printing bits of fixed-point numbers
static inline void print_bits(fixed_point_t num) {
    for (int i = DATA_WIDTH-1; i >= 0; i--) { // Start from the most significant bit
        printf("%d", (num >> i) & 1); // Extract and print each bit
        if (i % 4 == 0) { // Optional: Add a space for readability every 4 bits
            printf(" ");
        }
    }
    printf("\n");
}

// Helper function for printing bits of fixed-point product numbers
static inline void print_bits_product(fixed_point_product_t num) {
    for (int i = PRODUCT_DATA_WIDTH-1 ; i >= 0; i--) { // Start from the most significant bit
        printf("%d", (num >> i) & 1); // Extract and print each bit
        if (i % 4 == 0) { // Optional: Add a space for readability every 4 bits
            printf(" ");
        }
    }
    printf("\n");
}

// Helper function for fixed-point multiplication
static inline fixed_point_t fixed_mul(fixed_point_t a, fixed_point_t b) {
    return (fixed_point_t) (((fixed_point_product_t)a * b) >> FRACT_WIDTH);
}

// Helper function for fixed-point division
static inline fixed_point_t fixed_div(fixed_point_t a, fixed_point_t b) {
    if (b == 0) return 0;
    return (fixed_point_t) (((fixed_point_product_t)a << FRACT_WIDTH) / b);
}

// Converts a double to a fixed-point number
inline fixed_point_t double_to_fixed(double value) {    
    return (fixed_point_t)(value * (1 << FRACT_WIDTH));  // Multiply by 2^FRACT_WIDTH and round
}

// Converts a fixed-point number to a double
inline double fixed_to_double(fixed_point_t value) {
    return (double)value / (1 << FRACT_WIDTH);
}


#endif // FIXED_H 