#include "../inc/main.h"

int main(){

    int time = 0;
    Actuator_S actuator = {0};
    actuator.a10_r = double_to_fixed(1);     // (no distortion) for the first block of data  

    for (int j = 0; j < NUM_SETS; j++)
    {
        #ifdef PROFILE
            // start performance counters
            perf_reset();
            perf_start();
            reset_timer();
            start_timer();
        #endif
    
    
        fixed_point_t yhy_11_acumalator_arr[SET_SIZE/100] = {0};
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

        // compute YHY & YHE in the same for loop to remove redundant operations
        for (int i = 0; i < SET_SIZE; i ++ ) {

            //load from arrays to temp
            fixed_point_t y_r_temp = y_r_arr[j*SET_SIZE + i];
            fixed_point_t y_i_temp = y_i_arr[j*SET_SIZE + i];

            //fixed_point_t yp_r_temp = yp_r_arr[j*SET_SIZE + i];
            //fixed_point_t yp_i_temp = yp_i_arr[j*SET_SIZE + i];

            fixed_point_t xp_r_temp = xp_r_arr[j*SET_SIZE + i];
            fixed_point_t xp_i_temp = xp_i_arr[j*SET_SIZE + i];

            ///////////////////////Post Distorter///////////////////////

            // Load input 
            actuator.in_r = y_r_temp;
            actuator.in_i = y_i_temp;
        
            // Perform DPD operation
            actuator_func(&actuator);
        
            // Y absolute squared and power 4 for both YHY & YHE
            fixed_point_t in_r_2 = fixed_mul(y_r_temp,y_r_temp);
            fixed_point_t in_i_2 = fixed_mul(y_i_temp,y_i_temp);
            fixed_point_t y_2_temp =  in_r_2 + in_i_2;
            fixed_point_t y_4_temp = fixed_mul(y_2_temp,y_2_temp);

            // for YHY 
            fixed_point_t y_6_temp = fixed_mul(y_2_temp,y_4_temp);
            fixed_point_t y_8_temp = fixed_mul(y_4_temp,y_4_temp);
            fixed_point_t y_10_temp = fixed_mul(y_4_temp,y_6_temp);
        
            // for YHE
            //error for YHE   e = x (pre-distorted) - y (post-distorted) 
            fixed_point_t e_r = xp_r_temp - actuator.out_r;
            fixed_point_t e_i = xp_i_temp - actuator.out_i;

            // y* e =  (yr - j yi) (er + j ei) = (yr er + yi ei ) + j (yr ei - yi er)
            fixed_point_t temp_r = fixed_mul(y_r_temp, e_r) + fixed_mul(y_i_temp, e_i); 
            fixed_point_t temp_i = fixed_mul(y_r_temp, e_i) - fixed_mul(y_i_temp, e_r); 

            // accumulate the YHY and YHE values
            //YHY
            yhy_11 += y_2_temp;
            yhy_12 += y_4_temp;
            yhy_13 += y_6_temp; 
            yhy_23 += y_8_temp;
            yhy_33 += y_10_temp;
            if (i != 0 && i % 100 == 0) {
                yhy_11_acumalator_arr[i / 100 - 1] = yhy_11;
                yhy_11 = 0;
            }
        

            //YHE
            yhe_1_r += temp_r;
            yhe_1_i += temp_i; 
            yhe_2_r += fixed_mul(y_2_temp,temp_r);
            yhe_2_i += fixed_mul(y_2_temp,temp_i);
            yhe_3_r += fixed_mul(y_4_temp,temp_r);
            yhe_3_i += fixed_mul(y_4_temp,temp_i);
        
        } // end of SET_SIZE loop

        //divide by 4
        yhy_11 = yhy_11 >> 2;
        for (size_t i = 0; i < (SET_SIZE /100); i++)
        {
            yhy_11 += yhy_11_acumalator_arr[i] >> 2 ;
        }  

        // now we have all the YHY and YHE values accumulated
        // We need to calculate the YHY inverse matrix and the YHE matrix

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

        // YHE matrix
        // Note: We are using a 3x3 matrix to store the YHE values
        Matrix_S yhe_mat = {0};
        yhe_mat.a11 = yhe_1_r;
        yhe_mat.a12 = yhe_1_i;
        yhe_mat.a13 = yhe_2_r;
        yhe_mat.a22 = yhe_2_i;
        yhe_mat.a23 = yhe_3_r;
        yhe_mat.a33 = yhe_3_i;

        // YHY  matrix
        // Note: We are using a 3x3 matrix to store the YHY values
        // The YHY matrix is symmetric, so we only need to store the upper triangular part
        // NOTE: the YHY values are multiplied by 0.25 (1/4) before inverting 
        Matrix_S yhy_mat = {0};
        yhy_mat.a11 = yhy_11 ;           // yhy_11 is already divided by 4 
        yhy_mat.a12 = (yhy_12 >> 2);
        yhy_mat.a13 = (yhy_13 >> 2);
        yhy_mat.a23 = (yhy_23 >> 2);
        yhy_mat.a33 = (yhy_33 >> 2);

        
        Matrix_S yhy_inv = {0};

        int yhy_inv_status = mat_inv(&yhy_mat, &yhy_inv);  
        if (yhy_inv_status != 0) {
            printf("Error: YHY matrix inversion failed (det = 0) \n");
            return yhy_inv_status;
        }

        //update coefficients of the distorter
        coeff_update(&yhy_inv, &yhe_mat, &actuator);
        
        #ifdef PROFILE
            stop_timer();
            time = get_time();
            // stop performance counters
            perf_stop();
            printf("set %d) took %d cycles on single core \n",j+1, time);
        #endif

        printf("Set %d: Coefficients updated:\n", j);
    } // end of NUM_SETS loop



    return 0;
}