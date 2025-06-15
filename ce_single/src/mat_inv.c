#include "../inc/mat_inv.h"


// Invert symmetric 3x3 matrix represented by 6 Q4.12 values
int8_t mat_inv(const Matrix_S* in_mat, Matrix_S* out_mat) {
    // Compute determinant using fixed-point arithmetic
    // Break down into smaller operations to avoid overflow
    fixed_point_t a11_a31 = fixed_mul(mat->a11, mat->a31);
    fixed_point_t a11_a32 = fixed_mul(mat->a11, mat->a32);
    fixed_point_t a11_a33 = fixed_mul(mat->a11, mat->a33);

    fixed_point_t a21_a21 = fixed_mul(mat->a21, mat->a21);
    fixed_point_t a21_a31 = fixed_mul(mat->a21, mat->a31);
    fixed_point_t a21_a32 = fixed_mul(mat->a21, mat->a32);
    fixed_point_t a21_a33 = fixed_mul(mat->a21, mat->a33);
    
    fixed_point_t a31_a31 = fixed_mul(mat->a31, mat->a31);
    fixed_point_t a31_a32 = fixed_mul(mat->a31, mat->a32);
    fixed_point_t a31_a33 = fixed_mul(mat->a31, mat->a33);
    fixed_point_t a32_a32 = fixed_mul(mat->a32, mat->a32);
    
    fixed_point_t det0 = a11_a33 + a21_a32 + a21_a32 - a31_a31;
    fixed_point_t det1 = fixed_mul(mat->a31 , det0);
    fixed_point_t det2 = fixed_mul(mat->a11. a32_a32);
    fixed_point_t det3 = fixed_mul(mat->a33, a21_a21);
    fixed_point_t det = det1 - det2 - det3;
    // Check if determinant is zero
    if (det == 0) {
        return -1; // Matrix is singular, cannot invert
    }

    // Compute adjugate matrix using fixed-point arithmetic
    Matrix_S adj = {0};

    adj->a11 =  a31_a33 - a32_a32;
    adj->a21 =  a31_a32 - a21_a33;
    adj->a22 =  a11_a33 - a31_a31;
    adj->a31 =  a21_a32 - a31_a31;
    adj->a32 =  a21_a32 - a31_a31;
    adj->a33 =  a11_a31 - a21_a21;
    
    
    // Compute inverse using adjugate and determinant
    inv->a11 = fixed_div(adj->a11, det);
    inv->a21 = fixed_div(adj->a21, det);    
    inv->a22 = fixed_div(adj->a22, det);
    inv->a31 = fixed_div(adj->a31, det);    
    inv->a32 = fixed_div(adj->a32, det);
    inv->a33 = fixed_div(adj->a33, det); 

    return 0;
}

// MAIN: Load inputs and compare with output.txt
