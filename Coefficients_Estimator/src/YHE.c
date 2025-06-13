#include "../inc/main.h"


fixed_point_t yhe_1_r [NUM_THREADS];
fixed_point_t yhe_1_i [NUM_THREADS];
fixed_point_t yhe_2_r [NUM_THREADS];
fixed_point_t yhe_2_i [NUM_THREADS];
fixed_point_t yhe_3_r [NUM_THREADS];
fixed_point_t yhe_3_i [NUM_THREADS];
    
int YHE(fixed_point_t *y_r, fixed_point_t *y_i, fixed_point_t *yp_r, fixed_point_t *yp_i, fixed_point_t *xp_r, fixed_point_t *xp_i, yhe_S *yhe ) {
    // Get the core ID of the current thread
    int core_id = rt_core_id();
    int num_cores = get_core_num();
    printf("Core ID: %d, Number of cores: %d\n", core_id, num_cores);
    
    // Synchronize all threads before starting computation
    synch_barrier();
    
//    fixed_point_t e_r [NUM_ITERATIONS];
//    fixed_point_t e_i [NUM_ITERATIONS];
    


    for (int i = core_id; i < NUM_ITERATIONS; i += num_cores){
        fixed_point_t e_r = xp_r[i] - yp_r[i];
        fixed_point_t e_i = xp_i[i] - yp_i[i];

        fixed_point_t temp_y_r = y_r[i];
        fixed_point_t temp_y_i = y_i[i];
        

        fixed_point_t y_2 = fixed_mul_4(temp_y_r,temp_y_r, temp_y_i,temp_y_i);  
          
        // y* e =  (yr - j yi) (er + j ei) = (yr er + yi ei ) + j (yr ei - yi er)
        fixed_point_t temp_r = fixed_mul(temp_y_r, e_r) + fixed_mul(temp_y_i, e_i);         
        fixed_point_t temp_i = fixed_mul(temp_y_r, e_i) - fixed_mul(temp_y_i, e_r);
        
        fixed_point_t y_4 = fixed_mul(y_2,y_2);
        
        yhe_1_r[core_id] += temp_r;
        yhe_1_i[core_id] += temp_i; 
        yhe_2_r[core_id] += fixed_mul(y_2,temp_r);
        yhe_2_i[core_id] += fixed_mul(y_2,temp_i); 
        yhe_3_r[core_id] += fixed_mul(y_4,temp_r); 
        yhe_3_i[core_id] += fixed_mul(y_4,temp_i); 
    }
 
    synch_barrier();
    
    // Aggregate results based on core ID
    if (core_id == 0) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhe -> yhe_1_r += yhe_1_r[i];
        }
    }
    if (core_id == 1) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhe -> yhe_1_i += yhe_1_i[i];
        }
    }
    if (core_id == 2) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhe -> yhe_2_r += yhe_2_r[i];
        }
    }
    if (core_id == 3) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhe -> yhe_2_i += yhe_2_i[i];
        }
    }
    if (core_id == 4) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhe -> yhe_3_r += yhe_3_r[i];
        }
    }

    if (core_id == 5) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhe -> yhe_3_i += yhe_3_i[i];
        }
    }
    // Final synchronization
    synch_barrier();
    
    return 0;
}

