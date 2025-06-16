#include "../inc/main.h"

int main(){
    
    int yhy_11_correct_count = 0;
    int yhy_11_wrong_count = 0;
    int yhy_12_correct_count = 0;
    int yhy_12_wrong_count = 0;
    int yhy_13_correct_count = 0;
    int yhy_13_wrong_count = 0;
    int yhy_23_correct_count = 0;
    int yhy_23_wrong_count = 0;
    int yhy_33_correct_count = 0;
    int yhy_33_wrong_count = 0;
    int yhe_1_r_correct_count = 0;
    int yhe_1_r_wrong_count = 0;
    int yhe_1_i_correct_count = 0;
    int yhe_1_i_wrong_count = 0;
    int yhe_2_r_correct_count = 0;
    int yhe_2_r_wrong_count = 0;
    int yhe_2_i_correct_count = 0;
    int yhe_2_i_wrong_count = 0;
    int yhe_3_r_correct_count = 0;
    int yhe_3_r_wrong_count = 0;
    int yhe_3_i_correct_count = 0;
    int yhe_3_i_wrong_count = 0;
    
    int total_correct_count = 0;
    int total_wrong_count = 0;
    
    
    
    //Statistics_S correct_stats = {0};
    //Statistics_S wrong_stats = {0};
    
    fixed_point_t yhy_11  = 0;
    fixed_point_t yhy_12  = 0;
    fixed_point_t yhy_13  = 0;
    fixed_point_t yhy_23  = 0;
    fixed_point_t yhy_33  = 0;
 
    fixed_point_t yhe_1_r = 0;
    fixed_point_t yhe_1_i = 0;
    fixed_point_t yhe_2_r = 0;
    fixed_point_t yhe_2_i = 0;
    fixed_point_t yhe_3_r = 0;
    fixed_point_t yhe_3_i = 0;

    Actuator_S actuator = {0};
    actuator.a10_r = double_to_fixed(1);     // (no distortion) for the first block of data  

    printf("before for loop\n");
    // compute YHY & YHE in the same for loop to remove redundant operations
    for (int i = 0; i < NUM_ITERATIONS; i ++ ) {
        

        //load from arrays to temp

        // Y for DPD Y& HY & YHE
        fixed_point_t y_r_temp = y_r_arr[i];
        fixed_point_t y_i_temp = y_i_arr[i];
        //printf("y_r = %lf\n", fixed_to_double(y_r_temp));
        //printf("y_i = %lf\n", fixed_to_double(y_i_temp));


        ////////////////////////////YHY & YHE/////////////////////////////
        fixed_point_t yp_r_temp = yp_r_arr[i];
        fixed_point_t yp_i_temp = yp_i_arr[i];
        //printf("yp_r = %lf\n", fixed_to_double(yp_r_temp));
        //printf("yp_i = %lf\n", fixed_to_double(yp_i_temp));
        fixed_point_t xp_r_temp = xp_r_arr[i];
        fixed_point_t xp_i_temp = xp_i_arr[i];
        //printf("xp_r = %lf\n", fixed_to_double(xp_r_temp));
        //printf("xp_i = %lf\n", fixed_to_double(xp_i_temp));
    

        ///////////////////////Post Distorter///////////////////////
        
        // Load input and expected output
        actuator.in_r = y_r_temp;
        actuator.in_i = y_i_temp;
    
        // Perform DPD operation
        actuator_func(&actuator);

        // Store results in output arrays
        fixed_point_t yp_r_actual = actuator.out_r;
        fixed_point_t yp_i_actual = actuator.out_i;
        
    
        
/*        
        // Convert to double for analysis
        double in_r = fixed_to_double(actuator.in_r);
        double in_i = fixed_to_double(actuator.in_i);
        double exp_r_double = fixed_to_double(yp_r_temp);
        double exp_i_double = fixed_to_double(yp_i_temp);
        double out_r = fixed_to_double(actuator.out_r);
        double out_i = fixed_to_double(actuator.out_i);
        

        // Check if output matches expected
        if (abs(yp_r_temp - actuator.out_r) <= MARGIN && abs(yp_i_temp - actuator.out_i) <= MARGIN) {
            statistical_analysis(in_r, in_i, out_r, out_i, exp_r_double, exp_i_double, &correct_stats);
        } else {
            statistical_analysis(in_r, in_i, out_r, out_i, exp_r_double, exp_i_double, &wrong_stats);
        }
*/

        // Y absolute squared and power 4 for both YHY & YHE
        fixed_point_t in_r_2 = fixed_mul(y_r_temp,y_r_temp);
        //printf("in_r_2 = %lf\n", fixed_to_double(in_r_2));

        fixed_point_t in_i_2 = fixed_mul(y_i_temp,y_i_temp);
        //printf("in_i_2 = %lf\n", fixed_to_double(in_i_2));
        //print_bits(in_i_2);
        
        fixed_point_t y_2_temp =  in_r_2 + in_i_2;
        //printf("y_2 = %lf\n", fixed_to_double(y_2_temp));
        
        fixed_point_t y_4_temp = fixed_mul(y_2_temp,y_2_temp);
        //printf("y_4 = %lf\n", fixed_to_double(y_4_temp));
        // for YHY 
        fixed_point_t y_6_temp = fixed_mul(y_2_temp,y_4_temp);
        //printf("y_6 = %lf\n", fixed_to_double(y_6_temp));
        fixed_point_t y_8_temp = fixed_mul(y_4_temp,y_4_temp);
        //printf("y_8 = %lf\n", fixed_to_double(y_8_temp));
        fixed_point_t y_10_temp = fixed_mul(y_4_temp,y_6_temp);
        //printf("y_10 = %lf\n", fixed_to_double(y_10_temp));

        // for YHE
        
        //error for YHE   e = x (pre-distorted) - y (post-distorted) 
        fixed_point_t e_r = xp_r_temp - yp_r_actual;
        fixed_point_t e_i = xp_i_temp - yp_i_actual;
        
        //printf("e_r = %lf\n", fixed_to_double(e_r));
        //printf("e_i = %lf\n", fixed_to_double(e_i));

        // y* e =  (yr - j yi) (er + j ei) = (yr er + yi ei ) + j (yr ei - yi er)
        fixed_point_t mul1 = fixed_mul(y_r_temp, e_r);
        //printf("mul1 (yr*er) = %lf\n", fixed_to_double(mul1));
        
        fixed_point_t mul2 = fixed_mul(y_i_temp, e_i);
        //printf("mul2 (yi*ei) = %lf\n", fixed_to_double(mul2));
        
        fixed_point_t temp_r = mul1 + mul2; 
        //printf("temp_r(mul1+mul2) = %lf\n", fixed_to_double(temp_r));

        
        fixed_point_t mul3 = fixed_mul(y_r_temp, e_i);
        //printf("mul3 (yr*ei)= %lf\n", fixed_to_double(mul3));

        fixed_point_t mul4 = fixed_mul(y_i_temp, e_r);
        //printf("mul4 (yi*ei)= %lf\n", fixed_to_double(mul4));
        
        fixed_point_t temp_i = mul3 - mul4; 
        //printf("temp_i(mul3-mul4) = %lf\n", fixed_to_double(temp_i));

        //YHY
        yhy_11 += y_2_temp;
        yhy_12 += y_4_temp;
        yhy_13 += y_6_temp; 
        yhy_23 += y_8_temp;
        yhy_33 += y_10_temp;


        //YHE
        yhe_1_r += temp_r;
        yhe_1_i += temp_i; 
        yhe_2_r += fixed_mul(y_2_temp,temp_r);
        yhe_2_i += fixed_mul(y_2_temp,temp_i);
        yhe_3_r += fixed_mul(y_4_temp,temp_r);
        yhe_3_i += fixed_mul(y_4_temp,temp_i);


        ////////////////////////////////////////////YHY check//////////////////////////////////////////////////////////
        if(abs(yhy_11 - yhy_11_arr[i]) >= MARGIN ){
            printf(" %d) yhy 11 is wrong exp = %lf, actual = %lf\n", i, fixed_to_double(yhy_11_arr[i]), fixed_to_double(yhy_11));
            yhy_11_wrong_count++; 
        } else yhy_11_correct_count++;

        if(abs(yhy_12 - yhy_12_arr[i]) >= MARGIN ){
            printf(" %d) yhy 12 is wrong exp = %lf, actual = %lf\n", i, fixed_to_double(yhy_12_arr[i]), fixed_to_double(yhy_12));
            yhy_12_wrong_count++; 
        } else yhy_12_correct_count++;

        if(abs(yhy_13 - yhy_13_arr[i]) >= MARGIN ){
            printf(" %d) yhy 13 is wrong exp = %lf, actual = %lf\n", i, fixed_to_double(yhy_13_arr[i]), fixed_to_double(yhy_13));
            yhy_13_wrong_count++; 
        } else yhy_13_correct_count++;

        if(abs(yhy_23 - yhy_23_arr[i]) >= MARGIN ){
            printf(" %d) yhy 23 is wrong exp = %lf, actual = %lf\n", i, fixed_to_double(yhy_23_arr[i]), fixed_to_double(yhy_23));
            yhy_23_wrong_count++; 
        } else yhy_23_correct_count++;

        if(abs(yhy_33 - yhy_33_arr[i]) >= MARGIN ){
            printf(" %d) yhy 33 is wrong exp = %lf, actual = %lf\n", i, fixed_to_double(yhy_33_arr[i]), fixed_to_double(yhy_33));
            yhy_33_wrong_count++; 
        } else yhy_33_correct_count++;


        ///////////////////////////////////////YHE check///////////////////////////////////////////////////////////
        if(abs(yhe_1_r-yhe_1_r_arr[i]) >= MARGIN ){
            printf(" %d) yhe 1 r is wrong exp = %lf, actual = %lf\n", i, fixed_to_double(yhe_1_r_arr[i]), fixed_to_double(yhe_1_r));
            yhe_1_r_wrong_count++; 
        } else yhe_1_r_correct_count++;

        if(abs(yhe_1_i-yhe_1_i_arr[i]) >= MARGIN ){
            printf(" %d) yhe 1 i is wrong exp = %lf, actual = %lf\n", i, fixed_to_double(yhe_1_i_arr[i]), fixed_to_double(yhe_1_i));
            yhe_1_i_wrong_count++; 
        } else yhe_1_i_correct_count++;

        if(abs(yhe_2_r-yhe_2_r_arr[i]) >= MARGIN ){
            printf(" %d) yhe 2 r is wrong exp = %lf, actual = %lf\n", i, fixed_to_double(yhe_2_r_arr[i]), fixed_to_double(yhe_2_r));
            yhe_2_r_wrong_count++; 
        } else yhe_2_r_correct_count++;

        if(abs(yhe_2_i-yhe_2_i_arr[i]) >= MARGIN ){
            printf(" %d) yhe 2 i is wrong exp = %lf, actual = %lf\n", i, fixed_to_double(yhe_2_i_arr[i]), fixed_to_double(yhe_2_i));
            yhe_2_i_wrong_count++; 
        } else yhe_2_i_correct_count++;

        if(abs(yhe_3_r-yhe_3_r_arr[i]) >= MARGIN ){
            printf(" %d) yhe 3 r is wrong exp = %lf, actual = %lf\n", i, fixed_to_double(yhe_3_r_arr[i]), fixed_to_double(yhe_3_r));
            yhe_3_r_wrong_count++; 
        } else yhe_3_r_correct_count++;

        if(abs(yhe_3_i-yhe_3_i_arr[i]) >= MARGIN ){
            printf(" %d) yhe 3 i is wrong exp = %lf, actual = %lf\n", i, fixed_to_double(yhe_3_i_arr[i]), fixed_to_double(yhe_3_i));
            yhe_3_i_wrong_count++; 
        } else yhe_3_i_correct_count++;

    }

    printf("after for loop\n");

    printf("final YHE values:\n");
    printf("yhe_1_r = %lf\n", fixed_to_double(yhe_1_r));
    printf("yhe_1_i = %lf\n", fixed_to_double(yhe_1_i));
    printf("yhe_2_r = %lf\n", fixed_to_double(yhe_2_r));
    printf("yhe_2_i = %lf\n", fixed_to_double(yhe_2_i));
    printf("yhe_3_r = %lf\n", fixed_to_double(yhe_3_r));
    printf("yhe_3_i = %lf\n", fixed_to_double(yhe_3_i));

    printf("final YHY values:\n");
    printf("yhy_11 = %lf\n", fixed_to_double(yhy_11));
    printf("yhy_12 = %lf\n", fixed_to_double(yhy_12));
    printf("yhy_13 = %lf\n", fixed_to_double(yhy_13));
    printf("yhy_23 = %lf\n", fixed_to_double(yhy_23));
    printf("yhy_33 = %lf\n", fixed_to_double(yhy_33));
    
    // After we calculated all the YHY and YHE values we invert YHY
    Matrix_S yhy_matrix = {0};
    yhy_matrix.a11 = yhy_11;
    yhy_matrix.a12 = yhy_12;
    yhy_matrix.a13 = yhy_13;
    yhy_matrix.a23 = yhy_23;
    yhy_matrix.a33 = yhy_33;

    Matrix_S yhy_inv = {0};
    
    int yhy_inv_status = mat_inv(&yhy_matrix, &yhy_inv);  
    if (yhy_inv_status != 0) {
        printf("Error: YHY matrix inversion failed (det = 0) \n");
        return yhy_inv_status;
    }else
    {
        printf("YHY matrix inversion successful\n");
        printf("yhy_inv.a11 = %lf\n", fixed_to_double(yhy_inv.a11));
        printf("yhy_inv.a12 = %lf\n", fixed_to_double(yhy_inv.a12));
        printf("yhy_inv.a13 = %lf\n", fixed_to_double(yhy_inv.a13));
        printf("yhy_inv.a23 = %lf\n", fixed_to_double(yhy_inv.a23));
        printf("yhy_inv.a33 = %lf\n", fixed_to_double(yhy_inv.a33));
    }

    // place the YHE values in a matrix

/*
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
    Matrix_S yhe_mat = {0};
    yhe_mat.a11 = yhe_1_r;
    yhe_mat.a12 = yhe_1_i;
    yhe_mat.a13 = yhe_2_r;
    yhe_mat.a22 = yhe_2_i;
    yhe_mat.a23 = yhe_3_r;
    yhe_mat.a33 = yhe_3_i;

    //update coefficients of the distorter
    coeff_update(&yhy_inv, &yhe_mat, &actuator);

    printf("after coeff_update\n");
    printf("actuator.a10_r = %lf\n", fixed_to_double(actuator.a10_r));  
    printf("actuator.a10_i = %lf\n", fixed_to_double(actuator.a10_i));
    printf("actuator.a30_r = %lf\n", fixed_to_double(actuator.a30_r));
    printf("actuator.a30_i = %lf\n", fixed_to_double(actuator.a30_i));
    printf("actuator.a50_r = %lf\n", fixed_to_double(actuator.a50_r));
    printf("actuator.a50_i = %lf\n", fixed_to_double(actuator.a50_i));

    
    #ifdef TEST
    printf("Total yhy_11_correct = %d\n", yhy_11_correct_count);
    printf("Total yhy_11_wrong = %d\n",   yhy_11_wrong_count);
    printf("Total yhy_12_correct = %d\n", yhy_12_correct_count);
    printf("Total yhy_12_wrong = %d\n",   yhy_12_wrong_count);
    printf("Total yhy_13_correct = %d\n", yhy_13_correct_count);
    printf("Total yhy_13_wrong = %d\n",   yhy_13_wrong_count);
    printf("Total yhy_23_correct = %d\n", yhy_23_correct_count);
    printf("Total yhy_23_wrong = %d\n",   yhy_23_wrong_count);
    printf("Total yhy_33_correct = %d\n", yhy_33_correct_count);
    printf("Total yhy_33_wrong = %d\n",   yhy_33_wrong_count);
    
    printf("Total yhe_1_r_correct = %d\n", yhe_1_r_correct_count);
    printf("Total yhe_1_r_wrong = %d\n",   yhe_1_r_wrong_count);
    printf("Total yhe_1_i_correct = %d\n", yhe_1_i_correct_count);
    printf("Total yhe_1_i_wrong = %d\n",   yhe_1_i_wrong_count);
    printf("Total yhe_2_r_correct = %d\n", yhe_2_r_correct_count);
    printf("Total yhe_2_r_wrong = %d\n",   yhe_2_r_wrong_count);
    printf("Total yhe_2_i_correct = %d\n", yhe_2_i_correct_count);
    printf("Total yhe_2_i_wrong = %d\n",   yhe_2_i_wrong_count);
    printf("Total yhe_3_r_correct = %d\n", yhe_3_r_correct_count);
    printf("Total yhe_3_r_wrong = %d\n",   yhe_3_r_wrong_count);
    printf("Total yhe_3_i_correct = %d\n", yhe_3_i_correct_count);
    printf("Total yhe_3_i_wrong = %d\n",   yhe_3_i_wrong_count);

    total_correct_count = yhy_11_correct_count + yhy_12_correct_count + yhy_13_correct_count  + yhy_23_correct_count  + yhy_33_correct_count + yhe_1_r_correct_count + yhe_1_i_correct_count + yhe_2_r_correct_count + yhe_2_i_correct_count + yhe_3_r_correct_count + yhe_3_i_correct_count;
    total_wrong_count = yhy_11_wrong_count + yhy_12_wrong_count + yhy_13_wrong_count + yhy_23_wrong_count + yhy_33_wrong_count + yhe_1_r_wrong_count + yhe_1_i_wrong_count + yhe_2_r_wrong_count + yhe_2_i_wrong_count + yhe_3_r_wrong_count + yhe_3_i_wrong_count;
    printf("Total correct = %d\n", total_correct_count);
    printf("Total wrong = %d\n", total_wrong_count);

    //print_analysis_report(&correct_stats, &wrong_stats);
    #endif // TEST
    return 0;
}