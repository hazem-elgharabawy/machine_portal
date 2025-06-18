#include "../inc/main.h"
#include <math.h>  // Add this for fabs()

Actuator_S global_actuator = {
    .in_r = 0,
    .in_i = 0,
    .a10_r = 1 << FRACT_WIDTH ,    // (no distortion) for the first block of data
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


int main(){
    
    if (rt_cluster_id() != 0) {
        return bench_cluster_forward(0);
    }

    // Get the core ID of the current thread
    int core_id = rt_core_id();
    int num_cores = get_core_num();
    printf("Core ID: %d, Number of cores: %d\n", core_id, num_cores);

    Actuator_S actuator = {0};
    actuator = global_actuator;       // use the calculated coefficients

    // synchronize all cores before starting the computation
    synch_barrier();
    for (int j = 0; j < NUM_SETS; j++)
    {
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
        synch_barrier();

        // Each core will process a subset of the SET_SIZE elements
        // compute YHY & YHE in the same for loop to remove redundant operations
        for (int i = core_id; i < SET_SIZE; i += num_cores) {

            //load from arrays to temp
            fixed_point_t y_r_temp = y_r_arr[j*SET_SIZE + i];
            fixed_point_t y_i_temp = y_i_arr[j*SET_SIZE + i];

            fixed_point_t yp_r_temp = yp_r_arr[j*SET_SIZE + i];
            fixed_point_t yp_i_temp = yp_i_arr[j*SET_SIZE + i];

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
            printf("YHE MATRIX\n");
            printf("yhe_1_r  = %lf\n",fixed_to_double(yhe_mat.a11));
            printf("yhe_1_i  = %lf\n",fixed_to_double(yhe_mat.a12));
            printf("yhe_2_r  = %lf\n",fixed_to_double(yhe_mat.a13));
            printf("yhe_2_i  = %lf\n",fixed_to_double(yhe_mat.a22));
            printf("yhe_3_r  = %lf\n",fixed_to_double(yhe_mat.a23));
            printf("yhe_3_i  = %lf\n",fixed_to_double(yhe_mat.a33));
            // divide the YHY matrix by 4 (1/4) before inverting it
            // Note: yhy_11 is already divided by 4, so we don't need to divide it again
            yhy_mat.a12 = yhy_mat.a12 >> 2; // divide by 4
            yhy_mat.a13 = yhy_mat.a13 >> 2; // divide by 4
            yhy_mat.a23 = yhy_mat.a23 >> 2; // divide by 4
            yhy_mat.a33 = yhy_mat.a33 >> 2; // divide by 4

            printf("YHY MATRIX AFTER THE FACTOR\n");
            printf("yhy_11  = %lf\n",fixed_to_double(yhy_mat.a11));
            printf("yhy_12  = %lf\n",fixed_to_double(yhy_mat.a12));
            printf("yhy_13  = %lf\n",fixed_to_double(yhy_mat.a13));
            printf("yhy_23  = %lf\n",fixed_to_double(yhy_mat.a23));
            printf("yhy_33  = %lf\n",fixed_to_double(yhy_mat.a33));
            // Now we have the YHY and YHE matrices, we need to invert the YHY matrix
            
            // Create a matrix for the inverse of YHY            
            Matrix_S yhy_inv = {0};

            int yhy_inv_status = mat_inv(&yhy_mat, &yhy_inv);  
            if (yhy_inv_status != 0) {
                printf("Error: YHY matrix inversion failed (det = 0) \n");
                return yhy_inv_status;
            }


            // Now we have the YHY inverse matrix, we need to scale it by 0.25
            yhy_inv.a11 = yhy_inv.a11 >> 2; 
            yhy_inv.a12 = yhy_inv.a12 >> 2; 
            yhy_inv.a13 = yhy_inv.a13 >> 2; 
            yhy_inv.a22 = yhy_inv.a22 >> 2; 
            yhy_inv.a23 = yhy_inv.a23 >> 2; 
            yhy_inv.a33 = yhy_inv.a33 >> 2; 

            printf("INV AFTER THE FACTOR\n");
            printf("inv11  = %lf\n",fixed_to_double(yhy_inv.a11));
            printf("inv12  = %lf\n",fixed_to_double(yhy_inv.a12));
            printf("inv13  = %lf\n",fixed_to_double(yhy_inv.a13));
            printf("inv22  = %lf\n",fixed_to_double(yhy_inv.a22));
            printf("inv23  = %lf\n",fixed_to_double(yhy_inv.a23));
            printf("inv33  = %lf\n",fixed_to_double(yhy_inv.a33));
            //update coefficients of the distorter
            coeff_update(&yhy_inv, &yhe_mat, &global_actuator);

            printf("Set %d: Coefficients updated:\n", (j+1));
            printf("a10_r: %lf\n", fixed_to_double(global_actuator.a10_r));
            printf("a10_i: %lf\n", fixed_to_double(global_actuator.a10_i));
            printf("a30_r: %lf\n", fixed_to_double(global_actuator.a30_r));
            printf("a30_i: %lf\n", fixed_to_double(global_actuator.a30_i));
            printf("a50_r: %lf\n", fixed_to_double(global_actuator.a50_r));
            printf("a50_i: %lf\n", fixed_to_double(global_actuator.a50_i));

            printf("expected a10_r: %lf\n", fixed_to_double(a10_r_arr[j+1]));
            printf("expected a10_i: %lf\n", fixed_to_double(a10_i_arr[j+1]));
            printf("expected a30_r: %lf\n", fixed_to_double(a30_r_arr[j+1]));
            printf("expected a30_i: %lf\n", fixed_to_double(a30_i_arr[j+1]));
            printf("expected a50_r: %lf\n", fixed_to_double(a50_r_arr[j+1]));
            printf("expected a50_i: %lf\n", fixed_to_double(a50_i_arr[j+1]));

            // Convert fixed-point values to double before calculating error
            double expected_a10_r = fixed_to_double(a10_r_arr[j+1]);
            double actual_a10_r = fixed_to_double(global_actuator.a10_r);
            double expected_a10_i = fixed_to_double(a10_i_arr[j+1]);
            double actual_a10_i = fixed_to_double(global_actuator.a10_i);
            double expected_a30_r = fixed_to_double(a30_r_arr[j+1]);
            double actual_a30_r = fixed_to_double(global_actuator.a30_r);
            double expected_a30_i = fixed_to_double(a30_i_arr[j+1]);
            double actual_a30_i = fixed_to_double(global_actuator.a30_i);
            double expected_a50_r = fixed_to_double(a50_r_arr[j+1]);
            double actual_a50_r = fixed_to_double(global_actuator.a50_r);
            double expected_a50_i = fixed_to_double(a50_i_arr[j+1]);
            double actual_a50_i = fixed_to_double(global_actuator.a50_i);

            // Print raw values for debugging
            printf("\nDebug values:\n");
            printf("a10_r: expected=%lf, actual=%lf, diff=%lf\n", expected_a10_r, actual_a10_r, expected_a10_r - actual_a10_r);
            printf("a10_i: expected=%lf, actual=%lf, diff=%lf\n", expected_a10_i, actual_a10_i, expected_a10_i - actual_a10_i);
            printf("a30_r: expected=%lf, actual=%lf, diff=%lf\n", expected_a30_r, actual_a30_r, expected_a30_r - actual_a30_r);
            printf("a30_i: expected=%lf, actual=%lf, diff=%lf\n", expected_a30_i, actual_a30_i, expected_a30_i - actual_a30_i);
            printf("a50_r: expected=%lf, actual=%lf, diff=%lf\n", expected_a50_r, actual_a50_r, expected_a50_r - actual_a50_r);
            printf("a50_i: expected=%lf, actual=%lf, diff=%lf\n", expected_a50_i, actual_a50_i, expected_a50_i - actual_a50_i);
            printf("MARGIN=%lf\n", MARGIN);

            if (fabs(expected_a10_r - actual_a10_r) <= MARGIN) {
                printf("a10_r: correct with error = %lf\n", fabs(expected_a10_r - actual_a10_r));
            } else {
                printf("a10_r: wrong with error = %lf\n", fabs(expected_a10_r - actual_a10_r));
            }
        
            if (fabs(expected_a10_i - actual_a10_i) <= MARGIN) {
                printf("a10_i: correct with error = %lf\n", fabs(expected_a10_i - actual_a10_i));
            } else {
                printf("a10_i: wrong with error = %lf\n", fabs(expected_a10_i - actual_a10_i));
            }
        
            if (fabs(expected_a30_r - actual_a30_r) <= MARGIN) {
                printf("a30_r: correct with error = %lf\n", fabs(expected_a30_r - actual_a30_r));
            } else {
                printf("a30_r: wrong with error = %lf\n", fabs(expected_a30_r - actual_a30_r));
            }
        
            if (fabs(expected_a30_i - actual_a30_i) <= MARGIN) {
                printf("a30_i: correct with error = %lf\n", fabs(expected_a30_i - actual_a30_i));
            } else {
                printf("a30_i: wrong with error = %lf\n", fabs(expected_a30_i - actual_a30_i));
            }
        
            if (fabs(expected_a50_r - actual_a50_r) <= MARGIN) {
                printf("a50_r: correct with error = %lf\n", fabs(expected_a50_r - actual_a50_r));
            } else {
                printf("a50_r: wrong with error = %lf\n", fabs(expected_a50_r - actual_a50_r));
            }
            if (fabs(expected_a50_i - actual_a50_i) <= MARGIN) {
                printf("a50_i: correct with error = %lf\n", fabs(expected_a50_i - actual_a50_i));
            } else {
                printf("a50_i: wrong with error = %lf\n", fabs(expected_a50_i - actual_a50_i));
            }    
        }
        // synchronize all cores after updating the coefficients
        synch_barrier();

        // Update local actuator with global_actuator values
        actuator = global_actuator;

    } // end of NUM_SETS loop
    
    // Final synchronization to ensure all cores have completed their work
    synch_barrier();
    return 0;
}