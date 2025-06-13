#include "../inc/main.h"

// Allocate arrays for each core
    fixed_point_t y_2[NUM_THREADS];
    fixed_point_t y_4[NUM_THREADS];
    fixed_point_t y_6[NUM_THREADS];
    fixed_point_t y_8[NUM_THREADS];
    fixed_point_t y_10[NUM_THREADS];

int YHY(fixed_point_t *y_r, fixed_point_t *y_i, yhy_S *yhy ) {
    // Get the core ID of the current thread
    int core_id = rt_core_id();
    int num_cores = get_core_num();
    printf("Core ID: %d, Number of cores: %d\n", core_id, num_cores);
        
    y_2[core_id] = 0;
    y_4[core_id] = 0;
    y_6[core_id] = 0;
    y_8[core_id] = 0;
    y_10[core_id] = 0;
    
    // Synchronize all threads before starting computation
    synch_barrier();

    // Parallel computation of y_2
    for (int i = core_id; i < NUM_ITERATIONS; i += num_cores) {
        fixed_point_t temp_2 = fixed_mac(y_r[i], y_r[i], y_i[i], y_i[i]);        
        fixed_point_t temp_4 = fixed_mul(temp_2,temp_2);
        fixed_point_t temp_6 = fixed_mul(temp_2,temp_4);
        fixed_point_t temp_8 = fixed_mul(temp_4,temp_4);
        fixed_point_t temp_10 = fixed_mul(temp_4,temp_6);

        y_2[core_id]  += temp_2;
        y_4[core_id]  += temp_4;
        y_6[core_id]  += temp_6; 
        y_8[core_id]  += temp_8;
        y_10[core_id] += temp_10;
    } 
    
    synch_barrier();
    
    // Aggregate results based on core ID
    if (core_id == 0) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhy -> yhy_11 += y_2[i];
        }
    }
    if (core_id == 1) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhy -> yhy_12 += y_4[i];
        }
    }
    if (core_id == 2) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhy -> yhy_13 += y_6[i];
        }
    }
    if (core_id == 3) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhy -> yhy_23 += y_8[i];
        }
    }
    if (core_id == 4) {
        for (size_t i = 0; i < NUM_THREADS; i++) {
            yhy -> yhy_33 += y_10[i];
        }
    }

    // Final synchronization
    synch_barrier();
    
    return 0;
}

