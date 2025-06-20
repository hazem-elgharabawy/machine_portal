#ifndef FIXED_H
#define FIXED_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


// Fixed-point configuration
#define INT_WIDTH 6
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

// Helper function for fixed-point multiplication

static inline void print_bits_16(fixed_point_t num) {
    for (int i = 15; i >= 0; i--) { // Start from the most significant bit
        printf("%d", (num >> i) & 1); // Extract and print each bit
        if (i % 4 == 0) { // Optional: Add a space for readability every 4 bits
            printf(" ");
        }
    }
    printf("\n");
}
static inline void print_bits_32(fixed_point_product_t num) {
    for (int i = 31; i >= 0; i--) { // Start from the most significant bit
        printf("%d", (num >> i) & 1); // Extract and print each bit
        if (i % 4 == 0) { // Optional: Add a space for readability every 4 bits
            printf(" ");
        }
    }
    printf("\n");
}

static inline fixed_point_t fixed_mul(fixed_point_t a, fixed_point_t b) {
    fixed_point_product_t temp = a * b;
    /*
    printf("INSIDE FIXED MUL \n");
    printf("a = ");
    print_bits(a);
    printf("b = ");
    print_bits(b);
    printf("Product ");
    print_bits_product(temp);
    */
    return (fixed_point_t) (temp >> FRACT_WIDTH);
}

// Helper function for fixed-point division
static inline fixed_point_t fixed_div(fixed_point_t a, fixed_point_t b) {
    if (b == 0) return 0;
    return (fixed_point_t)((fixed_point_product_t)a << FRACT_WIDTH) / b;
}

// Helper function for fixed-point multiplication with addition
static inline fixed_point_t fixed_mul_4(fixed_point_t a, fixed_point_t b, fixed_point_t c, fixed_point_t d){
    fixed_point_product_t product = ((fixed_point_product_t)a * (fixed_point_product_t)b) + ((fixed_point_product_t)c * (fixed_point_product_t)d);
    product = product >> FRACT_WIDTH;
    return (fixed_point_t)product;
}

// Converts a double to a fixed-point number
inline fixed_point_t double_to_fixed(double value) {    
    return (fixed_point_t)(value * (1 << FRACT_WIDTH) + 0.5);  // Multiply by 2^FRACT_WIDTH and round
}

// Converts a fixed-point number to a double
inline double fixed_to_double(fixed_point_t value) {
    return (double)value / (1 << FRACT_WIDTH);
}


#endif // FIXED_H 