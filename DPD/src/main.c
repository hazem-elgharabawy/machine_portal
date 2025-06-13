#include "../inc/main.h"

// Use static variables for memory efficiency in embedded systems
static __attribute__((aligned(16))) fixed_point_t output_r[DATA_SIZE];
static __attribute__((aligned(16))) fixed_point_t output_i[DATA_SIZE];
#ifdef TEST 
#ifdef PARALLEL
static int thread_correct_counts[NUM_THREADS] = {0};
static int thread_wrong_counts[NUM_THREADS] = {0};
#endif
#endif
int main() {

#ifdef PARALLEL
    // Get the core ID of the current thread
    int core_id = rt_core_id(); // Function to get the core ID
    int num_cores = get_core_num();
    printf("Core ID: %d, Number of cores: %d\n", core_id, num_cores);
    synch_barrier(); // Synchronize all threads
    
    // Initialize actuator structure
    Actuator_S actuator = {0};
    #ifdef PROFILE
    enable_perf_cnt();
    config_perf_cnt();
    rst_start_perf_cnt();
    #endif
    
        for (int i = core_id; i < DATA_SIZE; i += num_cores) {
            
            // Load input and expected output
            actuator.in_r = input_r[i];
            actuator.in_i = input_i[i];
            
            // Perform DPD operation
            actuator_func_single(&actuator);
            // Store results in output arrays
            output_r[i] = actuator.out_r;
            output_i[i] = actuator.out_i;
            
            #ifdef TEST
            fixed_point_t exp_r = expected_r[i];
            fixed_point_t exp_i = expected_i[i];
            // Update thread-local counters
            if (abs(exp_r - actuator.out_r) <= MARGIN && abs(exp_i - actuator.out_i) <= MARGIN) {
                thread_correct_counts[core_id]++;
            } else {
                thread_wrong_counts[core_id]++;
            }
            #endif
        }
        synch_barrier();
        

    #ifdef PROFILE
    // Stop profiling and print results
    disable_perf_cnt();
    if (core_id == 0) {
        display_perf_cnt();
    }
    #endif
    #ifdef TEST
    if (core_id == 0) {
        int total_correct = 0;
        int total_wrong = 0;
        // Sum up thread-local counters
        for (int i = 0; i < NUM_THREADS; i++) {
        total_correct += thread_correct_counts[i];
        total_wrong += thread_wrong_counts[i];
        }
        // Print the results for the first core
        printf("Correct count: %d\n", total_correct);
        printf("Wrong count: %d\n", total_wrong);
    
    }
    #endif
    return 0;

#else
    // Single-threaded processing for non-parallel execution
    #ifdef TEST
    Statistics_S correct_stats = {0};
    Statistics_S wrong_stats = {0};
    #endif

    // Initialize actuator structure
    Actuator_S actuator = {0};  

    #ifdef PROFILE
        enable_perf_cnt();
        config_perf_cnt();
        rst_start_perf_cnt();
    #endif

    // Processing loop
    for (int i = 0; i < DATA_SIZE; i++) {
        
        // Load input and expected output
        actuator.in_r = input_r[i];
        actuator.in_i = input_i[i];
    
        // Perform DPD operation
        actuator_func_single(&actuator);

        // Store results in output arrays
        output_r[i] = actuator.out_r;
        output_i[i] = actuator.out_i;
        
        #ifdef TEST
        fixed_point_t exp_r = expected_r[i];
        fixed_point_t exp_i = expected_i[i];
        // Convert to double for analysis
        double in_r = fixed_to_double(actuator.in_r);
        double in_i = fixed_to_double(actuator.in_i);
        double exp_r_double = fixed_to_double(exp_r);
        double exp_i_double = fixed_to_double(exp_i);
        double out_r = fixed_to_double(actuator.out_r);
        double out_i = fixed_to_double(actuator.out_i);

        // Check if output matches expected
        if (abs(exp_r - actuator.out_r) <= MARGIN && abs(exp_i - actuator.out_i) <= MARGIN) {
            statistical_analysis(in_r, in_i, out_r, out_i, exp_r_double, exp_i_double, &correct_stats);
        } else {
            statistical_analysis(in_r, in_i, out_r, out_i, exp_r_double, exp_i_double, &wrong_stats);
        }
        #endif
    }

    #ifdef PROFILE
        // Stop profiling and print results
        disable_perf_cnt();
        display_perf_cnt();
    #endif

    #ifdef TEST
        print_analysis_report(&correct_stats, &wrong_stats);
    #endif

#endif
    
    return 0;  // Return success
}

