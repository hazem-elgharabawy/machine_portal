#include "../inc/actuator.h"
#include "../inc/mat_inv.h"
#include "../inc/coeff_update.h"

/*

                YHY                *           YHE           =          H
    | yhy_11 | yhy_12 | yhy_13 |        | yhe1r + yhe1i |       | a_10_r + a_10_i |
    | yhy_12 | yhy_22 | yhy_23 |        | yhe2r + yhe2i |       | a_30_r + a_30_i |
    | yhy_13 | yhy_23 | yhy_33 |        | yhe3r + yhe3i |       | a_50_r + a_50_i |



*********************************************************************************************************
yhe_mat is a 3x1 matrix with the following structure:

hye_mat =  
    | yhe1r + yhe1i |
    | yhe2r + yhe2i |
    | yhe3r + yhe3i |
                    3x1 matrix


HOWEVER  we will save it in a 3x3 symmetric matrix just to use the same structure Matrix_S for simplicity
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
************************************************************************************************************
*/



void coeff_update(const Matrix_S* yhy_inv,const Matrix_S* yhe_mat, Actuator_S* actuator){
    // a10_r        +=           inv11 * yhe_1_r             +               inv12 * yhe_2_r         +       inv13 * yhe_3_r
    actuator->a10_r += fixed_mul(yhy_inv->a11, yhe_mat->a11) + fixed_mul(yhy_inv->a12, yhe_mat->a13) + fixed_mul(yhy_inv->a13, yhe_mat->a23);
    
    // a10_i        +=           inv11 * yhe_1_i             +               inv12 * yhe_2_i         +       inv13 * yhe_3_i
    actuator->a10_i += fixed_mul(yhy_inv->a11, yhe_mat->a12) + fixed_mul(yhy_inv->a12, yhe_mat->a22) + fixed_mul(yhy_inv->a13, yhe_mat->a33);
    
    // a30_r        +=           inv12 * yhe_1_r             +               inv22 * yhe_2_r         +       inv23 * yhe_3_r
    actuator->a30_r += fixed_mul(yhy_inv->a12, yhe_mat->a11) + fixed_mul(yhy_inv->a22, yhe_mat->a13) + fixed_mul(yhy_inv->a23, yhe_mat->a23);
    
    // a30_i        +=           inv12 * yhe_1_i             +               inv22 * yhe_2_i         +       inv23 * yhe_3_i
    actuator->a30_i += fixed_mul(yhy_inv->a12, yhe_mat->a12) + fixed_mul(yhy_inv->a22, yhe_mat->a22) + fixed_mul(yhy_inv->a23, yhe_mat->a33);
    
    // a50_r        +=           inv13 * yhe_1_r             +               inv23 * yhe_2_r         +       inv33 * yhe_3_r
    actuator->a50_r += fixed_mul(yhy_inv->a13, yhe_mat->a11) + fixed_mul(yhy_inv->a23, yhe_mat->a13) + fixed_mul(yhy_inv->a33, yhe_mat->a23);
    
    // a50_i        +=           inv13 * yhe_1_i             +               inv23 * yhe_2_i         +       inv33 * yhe_3_i
    actuator->a50_i += fixed_mul(yhy_inv->a13, yhe_mat->a12) + fixed_mul(yhy_inv->a23, yhe_mat->a22) + fixed_mul(yhy_inv->a33, yhe_mat->a33);

}