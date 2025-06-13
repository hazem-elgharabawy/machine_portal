#include "../inc/mat_inv.h"

// Invert symmetric 3x3 matrix represented by 6 Q4.12 values
int8_t invert3x3_q412(const fixed_point_t* mat6, fixed_point_t* out9) {
    fixed_point_t a31_a33 = fixed_mul(mat6[2],mat6[5]);
    fixed_point_t a32_a32 = fixed_mul(mat6[4],mat6[4]);
    fixed_point_t a21_a21 = fixed_mul(mat6[1],mat6[1]);
    fixed_point_t a31_a31 = fixed_mul(mat6[2],mat6[2]);
    fixed_point_t a31_a31 = fixed_mul(mat6[2],mat6[2]);

    fixed_point_t a11_a33 = fixed_mul(mat6[0],mat6[5]);
    fixed_point_t a11_a33 = fixed_mul(mat6[0],mat6[5]);
    fixed_point_t a21_a32 = fixed_mul(mat6[1],mat6[4]);
    fixed_point_t a21_a32 = fixed_mul(mat6[1],mat6[4]);

}

// MAIN: Load inputs and compare with output.txt
