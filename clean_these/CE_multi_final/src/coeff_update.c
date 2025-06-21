#include "../inc/coeff_update.h"

/*

                YHY                *           YHE           =          H
    | yhy_11 | yhy_12 | yhy_13 |        | yhe1r + yhe1i |       | a_10_r + a_10_i |
    | yhy_12 | yhy_22 | yhy_23 |        | yhe2r + yhe2i |       | a_30_r + a_30_i |
    | yhy_13 | yhy_23 | yhy_33 |        | yhe3r + yhe3i |       | a_50_r + a_50_i |



*************************************************************************************************************
yhe_mat is a 3x1 matrix with the following structure:

hye_mat =  
    | yhe1r + yhe1i |
    | yhe2r + yhe2i |
    | yhe3r + yhe3i |
                    3x1 matrix


**HOWEVER**  we will save it in a 3x3 symmetric matrix just to use the same structure Matrix_S for simplicity
    | yhe1r |  | yhe1i |  | yhe2r |
    | yhe1i |  | yhe2i |  | yhe3r |
    | yhe2r |  | yhe3r |  | yhe3i |
where:
    yhe_mat-> a11 = yhe1r
    yhe_mat-> a12 = yhe1i               *****THIS DOES NOT HAVE ANY MEANING, IT IS JUST A PLACEHOLDER*****
    yhe_mat-> a13 = yhe2r
    yhe_mat-> a22 = yhe2i
    yhe_mat-> a23 = yhe3r
    yhe_mat-> a33 = yhe3i
****************************************************************************************************************
*/



void coeff_update(const Matrix_S* yhy_inv,const Matrix_S* yhe_mat, Actuator_S* actuator){
    //         a10_r_temp         =           inv11 * yhe_1_r             +               inv12 * yhe_2_r         +       inv13 * yhe_3_r
    fixed_point_t a10_r_temp = fixed_mul(yhy_inv->a11, yhe_mat->a11) + fixed_mul(yhy_inv->a12, yhe_mat->a13) + fixed_mul(yhy_inv->a13, yhe_mat->a23); 
    //     a10_r    +=  a10 * 0.75 
    actuator->a10_r += (a10_r_temp >> 2) + (a10_r_temp >> 1);  
/*    
    printf("half_mul_11_r = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a11, yhe_mat->a11)));
    printf("half_mul12_r = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a12, yhe_mat->a13)));
    printf("half_mul13_r = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a13, yhe_mat->a23)));
    printf("****************************\n");
*/  
    // a10_i        +=           inv11 * yhe_1_i             +               inv12 * yhe_2_i         +       inv13 * yhe_3_i
    fixed_point_t a10_i_temp = fixed_mul(yhy_inv->a11, yhe_mat->a12) + fixed_mul(yhy_inv->a12, yhe_mat->a22) + fixed_mul(yhy_inv->a13, yhe_mat->a33); 
    actuator->a10_i += (a10_i_temp >> 2) + (a10_i_temp >> 1);
/*
    printf("half_mul11_i = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a11, yhe_mat->a12)));
    printf("half_mul12_i = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a12, yhe_mat->a22)));
    printf("half_mul13_i = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a13, yhe_mat->a33)));
    printf("****************************\n");
*/
    // a30_r        +=           inv12 * yhe_1_r             +               inv22 * yhe_2_r         +       inv23 * yhe_3_r
    fixed_point_t a30_r_temp = fixed_mul(yhy_inv->a12, yhe_mat->a11) + fixed_mul(yhy_inv->a22, yhe_mat->a13) + fixed_mul(yhy_inv->a23, yhe_mat->a23);
    actuator->a30_r += (a30_r_temp >> 2) + (a30_r_temp >> 1);
/*
    printf("half_mul21_r = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a12, yhe_mat->a11)));
    printf("half_mul22_r = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a22, yhe_mat->a13)));
    printf("half_mul23_r = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a23, yhe_mat->a23)));
    printf("****************************\n");
*/
    
    // a30_i        +=           inv12 * yhe_1_i             +               inv22 * yhe_2_i         +       inv23 * yhe_3_i
    fixed_point_t a30_i_temp = fixed_mul(yhy_inv->a12, yhe_mat->a12) + fixed_mul(yhy_inv->a22, yhe_mat->a22) + fixed_mul(yhy_inv->a23, yhe_mat->a33);
    actuator->a30_i += (a30_i_temp >> 2) + (a30_i_temp >> 1);
/*  
    printf("half_mul21_i = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a12, yhe_mat->a12)));
    printf("half_mul22_i = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a22, yhe_mat->a22)));
    printf("half_mul23_i = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a23, yhe_mat->a33)));
    printf("****************************\n");
*/
    // a50_r        +=           inv13 * yhe_1_r             +               inv23 * yhe_2_r         +       inv33 * yhe_3_r
    fixed_point_t a50_r_temp = fixed_mul(yhy_inv->a13, yhe_mat->a11) + fixed_mul(yhy_inv->a23, yhe_mat->a13) + fixed_mul(yhy_inv->a33, yhe_mat->a23); 
    actuator->a50_r += (a50_r_temp >> 2) + (a50_r_temp >> 1);
/*  
    printf("half_mul31_r = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a13, yhe_mat->a11)));
    printf("half_mul32_r = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a23, yhe_mat->a13)));
    printf("half_mul33_r  = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a33, yhe_mat->a23)));
    printf("****************************\n");
*/
    // a50_i        +=           inv13 * yhe_1_i             +               inv23 * yhe_2_i         +       inv33 * yhe_3_i
    fixed_point_t a50_i_temp = fixed_mul(yhy_inv->a13, yhe_mat->a12) + fixed_mul(yhy_inv->a23, yhe_mat->a22) + fixed_mul(yhy_inv->a33, yhe_mat->a33);
    actuator->a50_i += (a50_i_temp >> 2) + (a50_i_temp >> 1);
/*
    printf("half_mul31_i = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a13, yhe_mat->a12)));
    printf("half_mul32_i = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a23, yhe_mat->a22)));
    printf("half_mul33_i = %lf\n", fixed_to_double(fixed_mul(yhy_inv->a33, yhe_mat->a33)));
    printf("****************************\n");
*/
}

void coeff_update_parallel(const Matrix_S* yhy_inv, const Matrix_S* yhe_mat, Actuator_S* actuator) {
    int core_id = rt_core_id();
    int num_cores = get_core_num();

    // Shared memory for temporary results
    static __attribute__((aligned(16))) fixed_point_t temp_coeffs[6];

    // Parallel computation of the 6 temporary coefficients
    for (int i = core_id; i < 6; i += num_cores) {
        if (i == 0) { // a10_r_temp
            temp_coeffs[i] = fixed_mul(yhy_inv->a11, yhe_mat->a11) + fixed_mul(yhy_inv->a12, yhe_mat->a13) + fixed_mul(yhy_inv->a13, yhe_mat->a23);
        } else if (i == 1) { // a10_i_temp
            temp_coeffs[i] = fixed_mul(yhy_inv->a11, yhe_mat->a12) + fixed_mul(yhy_inv->a12, yhe_mat->a22) + fixed_mul(yhy_inv->a13, yhe_mat->a33);
        } else if (i == 2) { // a30_r_temp
            temp_coeffs[i] = fixed_mul(yhy_inv->a12, yhe_mat->a11) + fixed_mul(yhy_inv->a22, yhe_mat->a13) + fixed_mul(yhy_inv->a23, yhe_mat->a23);
        } else if (i == 3) { // a30_i_temp
            temp_coeffs[i] = fixed_mul(yhy_inv->a12, yhe_mat->a12) + fixed_mul(yhy_inv->a22, yhe_mat->a22) + fixed_mul(yhy_inv->a23, yhe_mat->a33);
        } else if (i == 4) { // a50_r_temp
            temp_coeffs[i] = fixed_mul(yhy_inv->a13, yhe_mat->a11) + fixed_mul(yhy_inv->a23, yhe_mat->a13) + fixed_mul(yhy_inv->a33, yhe_mat->a23);
        } else if (i == 5) { // a50_i_temp
            temp_coeffs[i] = fixed_mul(yhy_inv->a13, yhe_mat->a12) + fixed_mul(yhy_inv->a23, yhe_mat->a22) + fixed_mul(yhy_inv->a33, yhe_mat->a33);
        }
    }

    synch_barrier();

    if (core_id == 0) {
        actuator->a10_r += (temp_coeffs[0] >> 2) + (temp_coeffs[0] >> 1);
        actuator->a10_i += (temp_coeffs[1] >> 2) + (temp_coeffs[1] >> 1);
        actuator->a30_r += (temp_coeffs[2] >> 2) + (temp_coeffs[2] >> 1);
        actuator->a30_i += (temp_coeffs[3] >> 2) + (temp_coeffs[3] >> 1);
        actuator->a50_r += (temp_coeffs[4] >> 2) + (temp_coeffs[4] >> 1);
        actuator->a50_i += (temp_coeffs[5] >> 2) + (temp_coeffs[5] >> 1);
    }

    synch_barrier();
}