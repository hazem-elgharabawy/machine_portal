#include "../inc/fixed.h"

fixed_point_t fixed_mul(fixed_point_t a, fixed_point_t b) {
    fixed_point_product_t product = (fixed_point_product_t)a * (fixed_point_product_t)b;  // Perform multiplication
    product = product >> FRACT_WIDTH;  // Adjust for fractional width
    return (fixed_point_t)product;
}