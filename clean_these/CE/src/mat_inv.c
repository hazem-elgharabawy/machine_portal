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

    //det(mat) = a13 * [(a11*a33) + (a12*a23) + (a12*a23) − (a13*a13)] - [a11*a23*a23] - [a33*a12*a12]
    
    fixed_point_t det0 = a11_a33 + a12_a23 + a12_a23 - a13_a13; // [(a11*a33) + (a12*a23) + (a12*a23) − (a13*a13)]
    fixed_point_t det1 = fixed_mul(mat->a13, det0);             // a13 * det0
    fixed_point_t det2 = fixed_mul(mat->a11, a23_a23);          // [a11*a23*a23]
    fixed_point_t det3 = fixed_mul(mat->a33, a12_a12);          // [a33*a12*a12]
    
    fixed_point_t det = det1 - det2 - det3;
    
    det = det << 2;                                             // multiply the det by factor of 4 to avoid overflow  

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

    // Compute inverse using adjugate and determinant
    inv->a11 = fixed_div(adj.a11, det);                 
    inv->a12 = fixed_div(adj.a12, det);    
    inv->a22 = fixed_div(adj.a22, det);
    inv->a13 = fixed_div(adj.a13, det);    
    inv->a23 = fixed_div(adj.a23, det);
    inv->a33 = fixed_div(adj.a33, det); 

    return 0;
}

// Invert symmetric 3x3 matrix represented by 6 Q4.12 values in parallel
int8_t mat_inv_parallel(const Matrix_S* mat, Matrix_S* inv) {
    int core_id = rt_core_id();
    int num_cores = get_core_num();

    // Shared memory for intermediate products and result
    static __attribute__((aligned(16))) fixed_point_t products[11];
    static __attribute__((aligned(16))) Matrix_S adj;
    static __attribute__((aligned(16))) fixed_point_t det;
    static int8_t status;

    // Parallel computation of the 11 initial multiplications
    for (int i = core_id; i < 11; i += num_cores) {
        if (i == 0)  products[i] = fixed_mul(mat->a11, mat->a13);
        if (i == 1)  products[i] = fixed_mul(mat->a11, mat->a23);
        if (i == 2)  products[i] = fixed_mul(mat->a11, mat->a33);
        if (i == 3)  products[i] = fixed_mul(mat->a12, mat->a12);
        if (i == 4)  products[i] = fixed_mul(mat->a12, mat->a13);
        if (i == 5)  products[i] = fixed_mul(mat->a12, mat->a23);
        if (i == 6)  products[i] = fixed_mul(mat->a12, mat->a33);
        if (i == 7)  products[i] = fixed_mul(mat->a13, mat->a13);
        if (i == 8)  products[i] = fixed_mul(mat->a13, mat->a23);
        if (i == 9)  products[i] = fixed_mul(mat->a13, mat->a33);
        if (i == 10) products[i] = fixed_mul(mat->a23, mat->a23);
    }

    synch_barrier();

    if (core_id == 0) {
        // det(mat) = a13 * [(a11*a33) + (a12*a23) + (a12*a23) - (a13*a13)] - [a11*a23*a23] - [a33*a12*a12]
        fixed_point_t det0 = products[2] + products[5] + products[5] - products[7]; // a11_a33 + a12_a23 + a12_a23 - a13_a13
        fixed_point_t det1 = fixed_mul(mat->a13, det0);
        fixed_point_t det2 = fixed_mul(mat->a11, products[10]); // a11 * a23_a23
        fixed_point_t det3 = fixed_mul(mat->a33, products[3]);  // a33 * a12_a12
        det = det1 - det2 - det3;
        det = det << 4;

        if (det == 0) {
            status = -1;
        } else {
            status = 0;
        }

        // Compute adjugate matrix
        adj.a11 = products[9] - products[10]; // a13_a33 - a23_a23
        adj.a12 = products[8] - products[6];  // a13_a23 - a12_a33
        adj.a22 = products[2] - products[7];  // a11_a33 - a13_a13
        adj.a13 = products[5] - products[7];  // a12_a23 - a13_a13
        adj.a23 = products[4] - products[1];  // a12_a13 - a11_a23
        adj.a33 = products[0] - products[3];  // a11_a13 - a12_a12
    }

    synch_barrier();

    if (status == -1) {
        return -1;
    }

    // Parallel computation of the 6 divisions
    for (int i = core_id; i < 6; i += num_cores) {
        if (i == 0) inv->a11 = fixed_div(adj.a11, det);
        if (i == 1) inv->a12 = fixed_div(adj.a12, det);
        if (i == 2) inv->a22 = fixed_div(adj.a22, det);
        if (i == 3) inv->a13 = fixed_div(adj.a13, det);
        if (i == 4) inv->a23 = fixed_div(adj.a23, det);
        if (i == 5) inv->a33 = fixed_div(adj.a33, det);
    }

    synch_barrier();

    return 0;
}