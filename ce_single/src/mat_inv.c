#include "../inc/mat_inv.h"


// Invert symmetric 3x3 matrix represented by 6 Q4.12 values
int8_t mat_inv(const Matrix_S* in_mat, Matrix_S* out_mat) {
    // Compute determinant using fixed-point arithmetic
    // Break down into smaller operations to avoid overflow
    fixed_point_t a11_a13 = fixed_mul(mat->a11, mat->a13);
    fixed_point_t a11_a23 = fixed_mul(mat->a11, mat->a23);
    fixed_point_t a11_a33 = fixed_mul(mat->a11, mat->a33);

    fixed_point_t a12_a12 = fixed_mul(mat->a12, mat->a12);
    fixed_point_t a12_a13 = fixed_mul(mat->a12, mat->a13);
    fixed_point_t a12_a23 = fixed_mul(mat->a12, mat->a23);
    fixed_point_t a12_a33 = fixed_mul(mat->a12, mat->a33);
    
    fixed_point_t a13_a13 = fixed_mul(mat->a13, mat->a13);
    fixed_point_t a13_a33 = fixed_mul(mat->a13, mat->a33);
    fixed_point_t a13_a23 = fixed_mul(mat->a13, mat->a23);
    
    fixed_point_t a23_a23 = fixed_mul(mat->a23, mat->a23);
    
    fixed_point_t det0 = a11_a33 + a12_a23 + a12_a23 - a13_a13;
    fixed_point_t det1 = fixed_mul(mat->a13 , det0);
    fixed_point_t det2 = fixed_mul(mat->a11. a23_a23);
    fixed_point_t det3 = fixed_mul(mat->a33, a12_a12);
    fixed_point_t det = det1 - det2 - det3;
    // Check if determinant is zero
    if (det == 0) {
        return -1; // Matrix is singular, cannot invert
    }

    // Compute adjugate matrix using fixed-point arithmetic
    Matrix_S adj = {0};

    adj->a11 =  a13_a33 - a23_a23;
    adj->a12 =  a13_a23 - a12_a33;
    adj->a22 =  a11_a33 - a13_a13;
    adj->a13 =  a12_a23 - a13_a13;
    adj->a23 =  a12_a23 - a13_a13;
    adj->a33 =  a11_a13 - a12_a12;
    
    
    // Compute inverse using adjugate and determinant
    inv->a11 = fixed_div(adj->a11, det);
    inv->a12 = fixed_div(adj->a12, det);    
    inv->a22 = fixed_div(adj->a22, det);
    inv->a13 = fixed_div(adj->a13, det);    
    inv->a23 = fixed_div(adj->a23, det);
    inv->a33 = fixed_div(adj->a33, det); 

    return 0;
}

// MAIN: Load inputs and compare with output.txt
