#include "../inc/main.h"

#ifdef PARALLEL 
    // global actuator to hold the coeffictienct for all cores
    Actuator_S global_actuator = {
    .in_r = 0,
    .in_i = 0,
    .a10_r = (1<<FRACT_WIDTH),    // (no distortion) for the first block of data
    .a10_i = 0,
    .a30_r = 0,
    .a30_i = 0,
    .a50_r = 0,
    .a50_i = 0,
    .out_r = 0,
    .out_i = 0
};
//////////////////////////////SHARED ARRAYS////////////////////////////////

//////////////////////////////////YHY/////////////////////////////////////
static __attribute__((aligned(16))) fixed_point_t yhy_11_shared  [NUM_THREADS] ;
static __attribute__((aligned(16))) fixed_point_t yhy_12_shared  [NUM_THREADS] ;
static __attribute__((aligned(16))) fixed_point_t yhy_13_shared  [NUM_THREADS] ;
static __attribute__((aligned(16))) fixed_point_t yhy_23_shared  [NUM_THREADS] ;
static __attribute__((aligned(16))) fixed_point_t yhy_33_shared [NUM_THREADS] ;

/////////////////////////////////YHE////////////////////////////////////////
static __attribute__((aligned(16))) fixed_point_t yhe_1_r_shared [NUM_THREADS];
static __attribute__((aligned(16))) fixed_point_t yhe_1_i_shared [NUM_THREADS];
static __attribute__((aligned(16))) fixed_point_t yhe_2_r_shared [NUM_THREADS];
static __attribute__((aligned(16))) fixed_point_t yhe_2_i_shared [NUM_THREADS];
static __attribute__((aligned(16))) fixed_point_t yhe_3_r_shared [NUM_THREADS];
static __attribute__((aligned(16))) fixed_point_t yhe_3_i_shared [NUM_THREADS];

#endif

int main(){
#ifdef PARALLEL
            if (rt_cluster_id() != 0) {
        return bench_cluster_forward(0);
    }
    // Get the core ID of the current thread
    int core_id = rt_core_id();
    int num_cores = get_core_num();
    printf("Core ID: %d, Number of cores: %d\n", core_id, num_cores);

    Actuator_S actuator = {0};

    // synchronize all cores before starting the computation
    synch_barrier();
    for (int j = 0; j < NUM_SETS; j++)
    {
        #ifdef PROFILE
        // start performance counters
        perf_reset();
        perf_start();
        #endif
        if (core_id == 0 ){
            reset_timer();
            start_timer();
        
        }
#ifdef USE_SW_COEFFS
    actuator = global_actuator; // Use the calculated coefficients
#else
    #ifdef USE_MATLAB_COEFFS
        // Load coefficients from MATLAB arrays
        actuator.a10_r = double_to_fixed(a10_r_matlab_arr[j]);
        actuator.a10_i = double_to_fixed(a10_i_matlab_arr[j]);
        actuator.a30_r = double_to_fixed(a30_r_matlab_arr[j]);
        actuator.a30_i = double_to_fixed(a30_i_matlab_arr[j]);
        actuator.a50_r = double_to_fixed(a50_r_matlab_arr[j]);
        actuator.a50_i = double_to_fixed(a50_i_matlab_arr[j]);
    #else
        #ifdef USE_HW_COEFFS
            // Load coefficients from hardware arrays
            actuator.a10_r = a10_r_hw_arr[j];
            actuator.a10_i = a10_i_hw_arr[j];
            actuator.a30_r = a30_r_hw_arr[j];
            actuator.a30_i = a30_i_hw_arr[j];
            actuator.a50_r = a50_r_hw_arr[j];
            actuator.a50_i = a50_i_hw_arr[j];
        #endif // USE_HW_COEFFS
    #endif // USE_MATLAB_COEFFS
#endif // USE_SW_COEFFS

        // Initialize accumulators for YHY and YHE
        yhy_11_shared[core_id] = 0;
        yhy_12_shared[core_id] = 0;
        yhy_13_shared[core_id] = 0;
        yhy_23_shared[core_id] = 0;
        yhy_33_shared[core_id] = 0;
        yhe_1_r_shared[core_id] = 0;
        yhe_1_i_shared[core_id] = 0;
        yhe_2_r_shared[core_id] = 0;
        yhe_2_i_shared[core_id] = 0;
        yhe_3_r_shared[core_id] = 0;
        yhe_3_i_shared[core_id] = 0;


    
        // synchronize all cores before processing the current set
        //synch_barrier();

        // Each core will process a subset of the SET_SIZE elements
        // compute YHY & YHE in the same for loop to remove redundant operations
        for (int i = core_id; i < SET_SIZE; i += num_cores) {
                
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
            yhy_11_shared[core_id] += y_2_temp;
            yhy_12_shared[core_id] += y_4_temp;
            yhy_13_shared[core_id] += y_6_temp; 
            yhy_23_shared[core_id] += y_8_temp;
            yhy_33_shared[core_id] += y_10_temp;
    
            //YHE
            yhe_1_r_shared[core_id] += temp_r;
            yhe_1_i_shared[core_id] += temp_i; 
            yhe_2_r_shared[core_id] += fixed_mul(y_2_temp,temp_r);
            yhe_2_i_shared[core_id] += fixed_mul(y_2_temp,temp_i);
            yhe_3_r_shared[core_id] += fixed_mul(y_4_temp,temp_r);
            yhe_3_i_shared[core_id] += fixed_mul(y_4_temp,temp_i);
        
        } // end of SET_SIZE loop
        
        // synchronize all cores after processing the current set
        synch_barrier();


        // After the loop, we have accumulated the YHY and YHE values for the current set and place them in their matrices
        
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
        
        // NOTE: for the YHY matrix, we will divide by a value of 4 (1/4) before inverting it
        // we will divide yhy_11 by 4 before acuumulating to avoid overflow


        if (core_id == 0) {

            // Create matrices for YHY and YHE
            Matrix_S yhy_mat = {0};
            Matrix_S yhe_mat = {0};

            // Accumulate the results from all cores to the matrices
            for (int k = 0; k < num_cores; k++) {
            
                // Accumulate the YHY values
                yhy_mat.a11 += yhy_11_shared[k] >> 2; // divide by 4 before accumulating to avoid overflow
                yhy_mat.a12 += yhy_12_shared[k];
                yhy_mat.a13 += yhy_13_shared[k];
                yhy_mat.a23 += yhy_23_shared[k];
                yhy_mat.a33 += yhy_33_shared[k];

                // Accumulate the YHE values
                // NOTE: this is a 3x1 matrix, but we will save it in a 3x3 symmetric matrix
                yhe_mat.a11 += yhe_1_r_shared[k];
                yhe_mat.a12 += yhe_1_i_shared[k];
                yhe_mat.a13 += yhe_2_r_shared[k];
                yhe_mat.a22 += yhe_2_i_shared[k];
                yhe_mat.a23 += yhe_3_r_shared[k];
                yhe_mat.a33 += yhe_3_i_shared[k];
            } 
            
            // divide the YHY matrix by 4 (1/4) before inverting it
            // Note: yhy_11 is already divided by 4, so we don't need to divide it again
            yhy_mat.a12 = yhy_mat.a12 >> 2; // divide by 4
            yhy_mat.a13 = yhy_mat.a13 >> 2; // divide by 4
            yhy_mat.a23 = yhy_mat.a23 >> 2; // divide by 4
            yhy_mat.a33 = yhy_mat.a33 >> 2; // divide by 4
                
            #ifdef DEBUG
                printf("YHE MATRIX\n");
                printf("yhe_1_r  = %lf\n",fixed_to_double(yhe_mat.a11));
                printf("yhe_1_i  = %lf\n",fixed_to_double(yhe_mat.a12));
                printf("yhe_2_r  = %lf\n",fixed_to_double(yhe_mat.a13));
                printf("yhe_2_i  = %lf\n",fixed_to_double(yhe_mat.a22));
                printf("yhe_3_r  = %lf\n",fixed_to_double(yhe_mat.a23));
                printf("yhe_3_i  = %lf\n",fixed_to_double(yhe_mat.a33));
                printf("YHY MATRIX AFTER THE FACTOR\n");
                printf("yhy_11  = %lf\n",fixed_to_double(yhy_mat.a11));
                printf("yhy_12  = %lf\n",fixed_to_double(yhy_mat.a12));
                printf("yhy_13  = %lf\n",fixed_to_double(yhy_mat.a13));
                printf("yhy_23  = %lf\n",fixed_to_double(yhy_mat.a23));
                printf("yhy_33  = %lf\n",fixed_to_double(yhy_mat.a33));
            #endif
            // Now we have the YHY and YHE matrices, we need to invert the YHY matrix
            
            // Create a matrix for the inverse of YHY            
            Matrix_S yhy_inv = {0};

            int yhy_inv_status = mat_inv(&yhy_mat, &yhy_inv);  
            if (yhy_inv_status != 0) {
                printf("Error: YHY matrix inversion failed (det = 0) \n");
                return yhy_inv_status;
            }
 
            
            #ifdef DEBUG
            printf("INV\n");
            printf("inv11  = %lf\n",fixed_to_double(yhy_inv.a11));
            printf("inv12  = %lf\n",fixed_to_double(yhy_inv.a12));
            printf("inv13  = %lf\n",fixed_to_double(yhy_inv.a13));
            printf("inv22  = %lf\n",fixed_to_double(yhy_inv.a22));
            printf("inv23  = %lf\n",fixed_to_double(yhy_inv.a23));
            printf("inv33  = %lf\n",fixed_to_double(yhy_inv.a33));
            #endif
            
            //update coefficients of the distorter
            coeff_update(&yhy_inv, &yhe_mat, &global_actuator);
            
            #ifdef PROFILE
                if ( core_id == 0 ) {
                    stop_timer();
                    time = get_time();
                }    
                // stop performance counters
                perf_stop();
                if(core_id == 0) printf("set %d) took %d cycles on multi core\n",j+1,time);
            #endif
            #ifdef TEST
                // Get values for comparison
                double matlab_a10_r = a10_r_matlab_arr[j+1];
                double hw_a10_r = fixed_to_double(a10_r_hw_arr[j+1]);
                double sw_a10_r = fixed_to_double(global_actuator.a10_r);
                double matlab_a10_i = a10_i_matlab_arr[j+1];
                double hw_a10_i = fixed_to_double(a10_i_hw_arr[j+1]);
                double sw_a10_i = fixed_to_double(global_actuator.a10_i);
                double matlab_a30_r = a30_r_matlab_arr[j+1];
                double hw_a30_r = fixed_to_double(a30_r_hw_arr[j+1]);
                double sw_a30_r = fixed_to_double(global_actuator.a30_r);
                double matlab_a30_i = a30_i_matlab_arr[j+1];
                double hw_a30_i = fixed_to_double(a30_i_hw_arr[j+1]);
                double sw_a30_i = fixed_to_double(global_actuator.a30_i);
                double matlab_a50_r = a50_r_matlab_arr[j+1];
                double hw_a50_r = fixed_to_double(a50_r_hw_arr[j+1]);
                double sw_a50_r = fixed_to_double(global_actuator.a50_r);
                double matlab_a50_i = a50_i_matlab_arr[j+1];
                double hw_a50_i = fixed_to_double(a50_i_hw_arr[j+1]);
                double sw_a50_i = fixed_to_double(global_actuator.a50_i);

                // Print all coefficients
                printf("Set %d Coefficients:\n", (j+1));
                printf("a10_r: MATLAB=%.6f, SW=%.6f, HW=%.6f\n", matlab_a10_r, sw_a10_r, hw_a10_r);
                printf("a10_i: MATLAB=%.6f, SW=%.6f, HW=%.6f\n", matlab_a10_i, sw_a10_i, hw_a10_i);
                printf("a30_r: MATLAB=%.6f, SW=%.6f, HW=%.6f\n", matlab_a30_r, sw_a30_r, hw_a30_r);
                printf("a30_i: MATLAB=%.6f, SW=%.6f, HW=%.6f\n", matlab_a30_i, sw_a30_i, hw_a30_i);
                printf("a50_r: MATLAB=%.6f, SW=%.6f, HW=%.6f\n", matlab_a50_r, sw_a50_r, hw_a50_r);
                printf("a50_i: MATLAB=%.6f, SW=%.6f, HW=%.6f\n", matlab_a50_i, sw_a50_i, hw_a50_i);
                printf("(MATLAB - SW): a10_r=%.6f, a10_i=%.6f, a30_r=%.6f, a30_i=%.6f, a50_r=%.6f, a50_i=%.6f\n",
                        fabs(matlab_a10_r - sw_a10_r),
                        fabs(matlab_a10_i - sw_a10_i),
                        fabs(matlab_a30_r - sw_a30_r),
                        fabs(matlab_a30_i - sw_a30_i),
                        fabs(matlab_a50_r - sw_a50_r),
                        fabs(matlab_a50_i - sw_a50_i));

                    printf("(MATLAB - HW): a10_r=%.6f, a10_i=%.6f, a30_r=%.6f, a30_i=%.6f, a50_r=%.6f, a50_i=%.6f\n",
                        fabs(matlab_a10_r - hw_a10_r),
                        fabs(matlab_a10_i - hw_a10_i),
                        fabs(matlab_a30_r - hw_a30_r),
                        fabs(matlab_a30_i - hw_a30_i),
                        fabs(matlab_a50_r - hw_a50_r),
                        fabs(matlab_a50_i - hw_a50_i));
                // Calculate Mean Square Error (MSE) for all coefficients
                double mse_sw = 0.0;
                double mse_hw = 0.0;
                int num_coeffs = 6; // a10_r, a10_i, a30_r, a30_i, a50_r, a50_i

                // MSE between sw and MATLAB values
                mse_sw += (matlab_a10_r - sw_a10_r) * (matlab_a10_r - sw_a10_r);
                mse_sw += (matlab_a10_i - sw_a10_i) * (matlab_a10_i - sw_a10_i);
                mse_sw += (matlab_a30_r - sw_a30_r) * (matlab_a30_r - sw_a30_r);
                mse_sw += (matlab_a30_i - sw_a30_i) * (matlab_a30_i - sw_a30_i);
                mse_sw += (matlab_a50_r - sw_a50_r) * (matlab_a50_r - sw_a50_r);
                mse_sw += (matlab_a50_i - sw_a50_i) * (matlab_a50_i - sw_a50_i);
                mse_sw /= num_coeffs;

                // MSE between hardware and MATLAB values
                mse_hw += (matlab_a10_r - hw_a10_r) * (matlab_a10_r - hw_a10_r);
                mse_hw += (matlab_a10_i - hw_a10_i) * (matlab_a10_i - hw_a10_i);
                mse_hw += (matlab_a30_r - hw_a30_r) * (matlab_a30_r - hw_a30_r);
                mse_hw += (matlab_a30_i - hw_a30_i) * (matlab_a30_i - hw_a30_i);
                mse_hw += (matlab_a50_r - hw_a50_r) * (matlab_a50_r - hw_a50_r);
                mse_hw += (matlab_a50_i - hw_a50_i) * (matlab_a50_i - hw_a50_i);
                mse_hw /= num_coeffs;

                printf("Set %d: MSE (SW)=%.6f, MSE (HW)=%.6f, Ratio=%.3f\n", 
                    (j+1), mse_sw, mse_hw, mse_sw / mse_hw);
                
                if (mse_sw < mse_hw) {
                    printf("SW is %.1f%% more accurate\n", ((mse_hw - mse_sw) / mse_hw) * 100.0);
                } else if (mse_sw > mse_hw) {
                    printf("SW is %.1f%% less accurate\n", ((mse_sw - mse_hw) / mse_hw) * 100.0);
                } else {
                    printf(" = (equal)\n");
                }
                printf("======================================================\n");
        #endif
        }
        // synchronize all cores after updating the coefficients
        synch_barrier();

    } // end of NUM_SETS loop
    
    // Final synchronization to ensure all cores have completed their work
    synch_barrier();


    return 0;
#else
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
            #ifdef TEST
            // Get values for comparison
            double matlab_a10_r = a10_r_matlab_arr[j+1];
            double hw_a10_r = fixed_to_double(a10_r_hw_arr[j+1]);
            double sw_a10_r = fixed_to_double(global_actuator.a10_r);
            double matlab_a10_i = a10_i_matlab_arr[j+1];
            double hw_a10_i = fixed_to_double(a10_i_hw_arr[j+1]);
            double sw_a10_i = fixed_to_double(global_actuator.a10_i);
            double matlab_a30_r = a30_r_matlab_arr[j+1];
            double hw_a30_r = fixed_to_double(a30_r_hw_arr[j+1]);
            double sw_a30_r = fixed_to_double(global_actuator.a30_r);
            double matlab_a30_i = a30_i_matlab_arr[j+1];
            double hw_a30_i = fixed_to_double(a30_i_hw_arr[j+1]);
            double sw_a30_i = fixed_to_double(global_actuator.a30_i);
            double matlab_a50_r = a50_r_matlab_arr[j+1];
            double hw_a50_r = fixed_to_double(a50_r_hw_arr[j+1]);
            double sw_a50_r = fixed_to_double(global_actuator.a50_r);
            double matlab_a50_i = a50_i_matlab_arr[j+1];
            double hw_a50_i = fixed_to_double(a50_i_hw_arr[j+1]);
            double sw_a50_i = fixed_to_double(global_actuator.a50_i);

            // Print all coefficients
            printf("Set %d Coefficients:\n", (j+1));
            printf("a10_r: MATLAB=%.6f, SW=%.6f, HW=%.6f\n", matlab_a10_r, sw_a10_r, hw_a10_r);
            printf("a10_i: MATLAB=%.6f, SW=%.6f, HW=%.6f\n", matlab_a10_i, sw_a10_i, hw_a10_i);
            printf("a30_r: MATLAB=%.6f, SW=%.6f, HW=%.6f\n", matlab_a30_r, sw_a30_r, hw_a30_r);
            printf("a30_i: MATLAB=%.6f, SW=%.6f, HW=%.6f\n", matlab_a30_i, sw_a30_i, hw_a30_i);
            printf("a50_r: MATLAB=%.6f, SW=%.6f, HW=%.6f\n", matlab_a50_r, sw_a50_r, hw_a50_r);
            printf("a50_i: MATLAB=%.6f, SW=%.6f, HW=%.6f\n", matlab_a50_i, sw_a50_i, hw_a50_i);
            printf("(MATLAB - SW): a10_r=%.6f, a10_i=%.6f, a30_r=%.6f, a30_i=%.6f, a50_r=%.6f, a50_i=%.6f\n",
                    fabs(matlab_a10_r - sw_a10_r),
                    fabs(matlab_a10_i - sw_a10_i),
                    fabs(matlab_a30_r - sw_a30_r),
                    fabs(matlab_a30_i - sw_a30_i),
                    fabs(matlab_a50_r - sw_a50_r),
                    fabs(matlab_a50_i - sw_a50_i));

                printf("(MATLAB - HW): a10_r=%.6f, a10_i=%.6f, a30_r=%.6f, a30_i=%.6f, a50_r=%.6f, a50_i=%.6f\n",
                    fabs(matlab_a10_r - hw_a10_r),
                    fabs(matlab_a10_i - hw_a10_i),
                    fabs(matlab_a30_r - hw_a30_r),
                    fabs(matlab_a30_i - hw_a30_i),
                    fabs(matlab_a50_r - hw_a50_r),
                    fabs(matlab_a50_i - hw_a50_i));
            // Calculate Mean Square Error (MSE) for all coefficients
            double mse_sw = 0.0;
            double mse_hw = 0.0;
            int num_coeffs = 6; // a10_r, a10_i, a30_r, a30_i, a50_r, a50_i

            // MSE between sw and MATLAB values
            mse_sw += (matlab_a10_r - sw_a10_r) * (matlab_a10_r - sw_a10_r);
            mse_sw += (matlab_a10_i - sw_a10_i) * (matlab_a10_i - sw_a10_i);
            mse_sw += (matlab_a30_r - sw_a30_r) * (matlab_a30_r - sw_a30_r);
            mse_sw += (matlab_a30_i - sw_a30_i) * (matlab_a30_i - sw_a30_i);
            mse_sw += (matlab_a50_r - sw_a50_r) * (matlab_a50_r - sw_a50_r);
            mse_sw += (matlab_a50_i - sw_a50_i) * (matlab_a50_i - sw_a50_i);
            mse_sw /= num_coeffs;

            // MSE between hardware and MATLAB values
            mse_hw += (matlab_a10_r - hw_a10_r) * (matlab_a10_r - hw_a10_r);
            mse_hw += (matlab_a10_i - hw_a10_i) * (matlab_a10_i - hw_a10_i);
            mse_hw += (matlab_a30_r - hw_a30_r) * (matlab_a30_r - hw_a30_r);
            mse_hw += (matlab_a30_i - hw_a30_i) * (matlab_a30_i - hw_a30_i);
            mse_hw += (matlab_a50_r - hw_a50_r) * (matlab_a50_r - hw_a50_r);
            mse_hw += (matlab_a50_i - hw_a50_i) * (matlab_a50_i - hw_a50_i);
            mse_hw /= num_coeffs;

            printf("Set %d: MSE (SW)=%.6f, MSE (HW)=%.6f, Ratio=%.3f\n", 
                   (j+1), mse_sw, mse_hw, mse_sw / mse_hw);
            
            if (mse_sw < mse_hw) {
                printf("SW is %.1f%% more accurate\n", ((mse_hw - mse_sw) / mse_hw) * 100.0);
            } else if (mse_sw > mse_hw) {
                printf("SW is %.1f%% less accurate\n", ((mse_sw - mse_hw) / mse_hw) * 100.0);
            } else {
                printf(" = (equal)\n");
            }
            printf("======================================================\n");
        #endif
    } // end of NUM_SETS loop

    return 0;
#endif
}