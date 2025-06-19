#include "../inc/mat_inv.h"

/*

    mat is a symmetric 3x3 matrix
    |a11 a12 a13|
    |a21 a22 a23|
    |a31 a32 a33|

    where:
    a12 = a21 
    a13 = a22 =  a31
    a23 = a32
    so we can represent it as:
    mat =
    |a11 a12 a13|
    |a12 a22 a23|
    |a13 a23 a33|

    inv is the inverse of mat

    to find the inverse of a symmetric matrix, we can use the formula:
    inv(mat) = 1/det(mat) * adj(mat)

    where det(mat) is the determinant of mat and adj(mat) is the adjugate of mat
    
    we can benefit from the symmetry of the matrix to simplify the calculations.
    det(mat) = a13 * [(a11*a33) + (a12*a23) + (a12*a23) − (a13*a13)] - [a11*a23*a23] - [a33*a12*a12] 

    we can also benefit from the symmetry of the matrix to simplify the calculations.
    adj(mat) =  | (a13*a33) - (a23*a23)    (a13*a23) − (a12*a33)     (a12*a23) - (a13*a22) |
                | (a13*a23) − (a12*a33)    (a11*a33) - (a13*a13)     (a13*a12) − (a11*a23) |
                | (a12*a23) - (a13*a22)    (a13*a12) − (a11*a23)     (a11*a33) - (a12*a12) |


    NOTE: the inv matrix is also symmetric where:
    inv12 = inv21
    inv13 = inv31
    inv23 = inv32

    so we only need to compute 6 values for the inverse matrix.

*/ 


// Invert symmetric 3x3 matrix represented by 6 Q4.12 values
int8_t mat_inv(const Matrix_S* mat, Matrix_S* inv) {
    // Compute determinant using fixed-point arithmetic
    fixed_point_t a11_a13 = fixed_mul(mat->a11, mat->a13);
    fixed_point_t a11_a23 = fixed_mul(mat->a11, mat->a23);
    fixed_point_t a11_a33 = fixed_mul(mat->a11, mat->a33);

    fixed_point_t a12_a12 = fixed_mul(mat->a12, mat->a12);
    fixed_point_t a12_a13 = fixed_mul(mat->a12, mat->a13);
    fixed_point_t a12_a23 = fixed_mul(mat->a12, mat->a23);
    fixed_point_t a12_a33 = fixed_mul(mat->a12, mat->a33);
    
    fixed_point_t a13_a13 = fixed_mul(mat->a13, mat->a13);
    fixed_point_t a13_a23 = fixed_mul(mat->a13, mat->a23);
    fixed_point_t a13_a33 = fixed_mul(mat->a13, mat->a33);
    
    fixed_point_t a23_a23 = fixed_mul(mat->a23, mat->a23);
/*
    printf("a11_a13 = %lf\n", fixed_to_double(a11_a13));
    printf("a11_a23 = %lf\n", fixed_to_double(a11_a23));
    printf("a11_a33 = %lf\n", fixed_to_double(a11_a33));
    
    printf("a12_a12 = %lf\n", fixed_to_double(a12_a12));
    printf("a12_a13 = %lf\n", fixed_to_double(a12_a13));
    printf("a12_a23 = %lf\n", fixed_to_double(a12_a23));
    printf("a12_a33 = %lf\n", fixed_to_double(a12_a33));

    printf("a13_a13 = %lf\n", fixed_to_double(a13_a13));
    printf("a13_a23 = %lf\n", fixed_to_double(a13_a23));
    printf("a13_a33 = %lf\n", fixed_to_double(a13_a33));
    
    printf("a23_a23 = %lf\n", fixed_to_double(a23_a23));
*/

    //det(mat) = a13 * [(a11*a33) + (a12*a23) + (a12*a23) − (a13*a13)] - [a11*a23*a23] - [a33*a12*a12]
    
    fixed_point_t det0 = a11_a33 + a12_a23 + a12_a23 - a13_a13; // [(a11*a33) + (a12*a23) + (a12*a23) − (a13*a13)]
    fixed_point_t det1 = fixed_mul(mat->a13, det0);             // a13 * det0
    fixed_point_t det2 = fixed_mul(mat->a11, a23_a23);          // [a11*a23*a23]
    fixed_point_t det3 = fixed_mul(mat->a33, a12_a12);          // [a33*a12*a12]
    
    fixed_point_t det = det1 - det2 - det3;
    
    det = det << 4;                                             // multiply the det by factor of 32 to avoid overflow  
    //printf("det = %lf\n", fixed_to_double(det));
    // Check if determinant is zero
    if (det == 0) {
        return -1; // Matrix is singular, cannot invert
    }

    // Compute adjugate matrix using fixed-point arithmetic
    Matrix_S adj = {0};

    adj.a11 =  a13_a33 - a23_a23;
    adj.a12 =  a13_a23 - a12_a33; 
    adj.a22 =  a11_a33 - a13_a13;
    adj.a13 =  a12_a23 - a13_a13;
    adj.a23 =  a12_a13 - a11_a23; 
    adj.a33 =  a11_a13 - a12_a12;
/*
    printf("adj.a11 = %lf\n", fixed_to_double(adj.a11));
    printf("adj.a12 = %lf\n", fixed_to_double(adj.a12));
    printf("adj.a22 = %lf\n", fixed_to_double(adj.a22));
    printf("adj.a13 = %lf\n", fixed_to_double(adj.a13));
    printf("adj.a23 = %lf\n", fixed_to_double(adj.a23));
    printf("adj.a33 = %lf\n", fixed_to_double(adj.a33));
*/
    // Compute inverse using adjugate and determinant
    inv->a11 = fixed_div(adj.a11, det);                 
    inv->a12 = fixed_div(adj.a12, det);    
    inv->a22 = fixed_div(adj.a22, det);
    inv->a13 = fixed_div(adj.a13, det);    
    inv->a23 = fixed_div(adj.a23, det);
    inv->a33 = fixed_div(adj.a33, det); 

    return 0;
}
