#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "fixed.h"
#include "mat_inv.h"
#include "YHY.h"
#include "YHE.h"
#include "input_data.h"
#include "output_data.h"

#define NUM_THREADS 6
    #define NUM_ITERATIONS 1000

// Function declarations
// Static function to compare matrices with minimal memory usage
static inline int8_t compare_matrices(const fixed_point_t* actual, const fixed_point_t* expected, int8_t status) {
    for (int8_t i = 0; i < FULL_MATRIX_ELEMENTS; ++i) {
        if (actual[i] != expected[i]) {
            return -1;
        }
    }
    return status;
}
int main(void);

#endif // MAIN_H 