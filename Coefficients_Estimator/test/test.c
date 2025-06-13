#include "test.h"

// Test result structure
typedef struct {
    uint32_t total_tests;
    uint32_t passed_tests;
    uint32_t failed_tests;
} test_result_t;

// Initialize test result
static void init_test_result(test_result_t* result) {
    result->total_tests = 0;
    result->passed_tests = 0;
    result->failed_tests = 0;
}

// Compare matrices with tolerance
static int8_t compare_matrices(const fixed_point_t* actual, const fixed_point_t* expected) {
    for (int8_t i = 0; i < FULL_MATRIX_ELEMENTS; ++i) {
        if (abs(actual[i] - expected[i]) > 1) {  // Allow 1 LSB tolerance
            return 0;
        }
    }
    return 1;
}

// Run a single test case
static void run_test_case(const fixed_point_t* input, const fixed_point_t* expected, test_result_t* result) {
    fixed_point_t actual[FULL_MATRIX_ELEMENTS];
    
    // Print input matrix
    printf("\nTest case %d:\n", result->total_tests);
    printf("Input matrix:\n");
    for (int8_t i = 0; i < SYMMETRIC_MATRIX_ELEMENTS; ++i) {
        printf("%lf ", fixed_to_double(input[i]));
        if ((i + 1) % 3 == 0) printf("\n");
    }
    
    // Run matrix inversion
    int8_t status = invert3x3_q412(input, actual);
    
    // Check result
    if (status == 0 && compare_matrices(actual, expected)) {
        result->passed_tests++;
        printf("  Test passed\n");
    } else {
        result->failed_tests++;
        printf("  Test failed\n");
        if (status != 0) {
            printf("  Error: Matrix inversion failed with status %d\n", status);
        } else {
            printf("  Error: Matrix mismatch\n");
            printf("  Expected output:\n");
            for (int8_t i = 0; i < FULL_MATRIX_ELEMENTS; ++i) {
                printf("%lf ", fixed_to_double(expected[i]));
                if ((i + 1) % 3 == 0) printf("\n");
            }
            printf("  Actual output:\n");
            for (int8_t i = 0; i < FULL_MATRIX_ELEMENTS; ++i) {
                printf("%lf ", fixed_to_double(actual[i]));
                if ((i + 1) % 3 == 0) printf("\n");
            }
        }
    }
    
    result->total_tests++;
}

int main(void) {
    test_result_t result;
    init_test_result(&result);
    
    printf("Starting matrix inversion tests...\n");
    
    // Run all test cases
    for (uint32_t i = 0; i < NUM_TEST_MATRICES; ++i) {
        const fixed_point_t* input_matrix = &input[i * SYMMETRIC_MATRIX_ELEMENTS];
        const fixed_point_t* expected = &output[i * FULL_MATRIX_ELEMENTS];
        run_test_case(input_matrix, expected, &result);
    }
    
    // Print results
    printf("\nTest Results:\n");
    printf("Total tests: %d\n", result.total_tests);
    printf("Passed: %d\n", result.passed_tests);
    printf("Failed: %d\n", result.failed_tests);
    printf("Success rate: %.2f%%\n", 
           (float)result.passed_tests / result.total_tests * 100.0f);
    
    return (result.failed_tests == 0) ? 0 : 1;
} 