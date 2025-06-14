#include "../inc/main.h"


//////////////////////////////SHARED STRUCTURES/////////////////////////////
yhy_S yhy = {0};
yhe_S yhe = {0};
//////////////////////////////SHARED ARRAYS////////////////////////////////

//////////////////////////////////YHY/////////////////////////////////////
static __attribute__((aligned(16))) fixed_point_t y_2_arr  [NUM_THREADS] ;
static __attribute__((aligned(16))) fixed_point_t y_4_arr  [NUM_THREADS] ;
static __attribute__((aligned(16))) fixed_point_t y_6_arr  [NUM_THREADS] ;
static __attribute__((aligned(16))) fixed_point_t y_8_arr  [NUM_THREADS] ;
static __attribute__((aligned(16))) fixed_point_t y_10_arr [NUM_THREADS] ;

/////////////////////////////////YHE////////////////////////////////////////
static __attribute__((aligned(16))) fixed_point_t yhe_1_r_arr [NUM_THREADS];
static __attribute__((aligned(16))) fixed_point_t yhe_1_i_arr [NUM_THREADS];
static __attribute__((aligned(16))) fixed_point_t yhe_2_r_arr [NUM_THREADS];
static __attribute__((aligned(16))) fixed_point_t yhe_2_i_arr [NUM_THREADS];
static __attribute__((aligned(16))) fixed_point_t yhe_3_r_arr [NUM_THREADS];
static __attribute__((aligned(16))) fixed_point_t yhe_3_i_arr [NUM_THREADS];

int main(){

    // Get the core ID of the current thread
    int core_id = rt_core_id();
    int num_cores = get_core_num();
    printf("Core ID: %d, Number of cores: %d\n", core_id, num_cores);

    // clear the arrays        
    y_2_arr[core_id] = 0;
    y_4_arr[core_id] = 0;
    y_6_arr[core_id] = 0;
    y_8_arr[core_id] = 0;
    y_10_arr[core_id] = 0;

    yhe_1_r_arr[core_id] = 0;
    yhe_1_i_arr[core_id] = 0;
    yhe_2_r_arr[core_id] = 0;
    yhe_2_i_arr[core_id] = 0;
    yhe_3_r_arr[core_id] = 0;
    yhe_3_i_arr[core_id] = 0;
    
    // Initialize structures
    Actuator_S actuator = {0};
    
    
    // Synchronize all threads before starting computation
    synch_barrier();

    // compute YHY & YHE in the same for loop to remove redundant operations
    for (int i = core_id; i < NUM_ITERATIONS; i += num_cores) {
        
        // Y for DPD Y& HY & YHE
        fixed_point_t y_r_temp = y_r_arr[i];
        fixed_point_t y_i_temp = y_i_arr[i];

        ///////////////////post-distortion///////////////////////////////
    
        actuator.in_r = y_r_temp;
        actuator.in_i = y_i_temp;

        // Perform DPD operation
        actuator_func(&actuator);
        #ifdef TEST
        // Update thread-local counters
            if (abs(yp_r_arr[i] - actuator.out_r) <= MARGIN && abs(yp_i_arr[i
            ] - actuator.out_i) <= MARGIN) {
                thread_correct_counts[core_id]++;
            } else {
                thread_wrong_counts[core_id]++;
            }
        #endif
        
        
        //error for YHE   e = x (pre-distorted) - y (post-distorted) 
        fixed_point_t e_r = xp_r_arr[i] - actuator.out_r;
        fixed_point_t e_i = xp_i_arr[i] - actuator.out_i;
    
        // Y absolute squared and power 4 for both YHY & YHE 
        fixed_point_t y_2_temp = fixed_mul_4(y_r_temp,y_r_temp, y_i_temp, y_i_temp);
        fixed_point_t y_4_temp = fixed_mul(y_2_temp,y_2_temp);
        // for YHY 
        fixed_point_t y_6_temp = fixed_mul(y_2_temp,y_4_temp);
        fixed_point_t y_8_temp = fixed_mul(y_4_temp,y_4_temp);
        fixed_point_t y_10_temp = fixed_mul(y_4_temp,y_6_temp);

        // for YHE 
        // y* e =  (yr - j yi) (er + j ei) = (yr er + yi ei ) + j (yr ei - yi er)
        fixed_point_t temp_r = fixed_mul(y_r_temp, e_r) + fixed_mul(y_i_temp, e_i);         
        fixed_point_t temp_i = fixed_mul(y_r_temp, e_i) - fixed_mul(y_i_temp, e_r);
        
        //YHY
        y_2_arr[core_id]  += y_2_temp;
        y_4_arr[core_id]  += y_4_temp;
        y_6_arr[core_id]  += y_6_temp; 
        y_8_arr[core_id]  += y_8_temp;
        y_10_arr[core_id] += y_10_temp;

        //YHE
        yhe_1_r_arr[core_id] += temp_r;
        yhe_1_i_arr[core_id] += temp_i; 
        yhe_2_r_arr[core_id] += fixed_mul(y_2_temp,temp_r);
        yhe_2_i_arr[core_id] += fixed_mul(y_2_temp,temp_i);
        yhe_3_r_arr[core_id] += fixed_mul(y_4_temp,temp_r);
        yhe_3_i_arr[core_id] += fixed_mul(y_4_temp,temp_i);

    } 
    
    synch_barrier();
    
    // Aggregate results based on core ID
    if (core_id == 0) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhy.yhy_11 += y_2_arr[i];
            yhe.yhe_2_i += yhe_2_i_arr[i];
        }
    }
    else if (core_id == 1) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhy.yhy_12 += y_4_arr[i];
            yhe.yhe_3_r += yhe_3_r_arr[i];
        }
    }
    else if (core_id == 2) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhy.yhy_13 += y_6_arr[i];
            yhe.yhe_3_i += yhe_3_i_arr[i];
        }
    }
    else if (core_id == 3) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhy.yhy_23 += y_8_arr[i];
        }
    }
    else if (core_id == 4) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhy.yhy_33 += y_10_arr[i];
        }
    }
    else if (core_id == 5) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhe.yhe_1_r += yhe_1_r_arr[i];
        }
    }
    else if (core_id == 6) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhe.yhe_1_i += yhe_1_i_arr[i];
        }
    }
    else if(core_id == 7) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhe.yhe_2_r += yhe_2_r_arr[i];
        }
    }

    // Final synchronization
    synch_barrier();

}