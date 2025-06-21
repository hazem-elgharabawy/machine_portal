#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "pulp.h"
#include "fixed.h"
#include "actuator.h"
#include "mat_inv.h"

#include "arrays/sync_pa_out_i.h"
#include "arrays/sync_pa_out_r.h"
#include "arrays/sync_pa_in_i.h"
#include "arrays/sync_pa_in_r.h"


#include "arrays/outer_loop/a10_r_matlab.h"
#include "arrays/outer_loop/a10_i_matlab.h"
#include "arrays/outer_loop/a30_r_matlab.h"
#include "arrays/outer_loop/a30_i_matlab.h"
#include "arrays/outer_loop/a50_r_matlab.h"
#include "arrays/outer_loop/a50_i_matlab.h"

#include "arrays/outer_loop/a10_r_hw.h"
#include "arrays/outer_loop/a10_i_hw.h"
#include "arrays/outer_loop/a30_r_hw.h"
#include "arrays/outer_loop/a30_i_hw.h"
#include "arrays/outer_loop/a50_r_hw.h"
#include "arrays/outer_loop/a50_i_hw.h"




//#define PARALLEL
#define NUM_SETS 6
#define NUM_THREADS 8
#define SET_SIZE 500
#define USE_SW_COEFFS
//#define USE_HW_COEFFS
//#define USE_MATLAB_COEFFS
#define PROFILE
//#define DEBUG     // Uncomment for debug mode to print intermediate values
//#define TEST      // Uncomment for test mode to include test-specific headers and variables



typedef struct {
    int count;
    double sum_r, sum_i, sum_r_squared, sum_i_squared;
    double max_r, max_i, min_r, min_i;
} Statistics_S;


static inline int statistical_analysis(double in_r, double in_i, double out_r, double out_i, double expected_out_r, double expected_out_i, Statistics_S *stats) {
    // Update the statistical analysis variables
    (stats->count)++;
    (stats->sum_r) += in_r;
    (stats->sum_i) += in_i;
    (stats->sum_r_squared) += in_r * in_r;
    (stats->sum_i_squared) += in_i * in_i;
    if (in_r > (stats->max_r)) {
        (stats->max_r) = in_r;
    }
    if (in_i > (stats->max_i)) {
        (stats->max_i) = in_i;
    }
    if (in_r < (stats->min_r)) {
        (stats->min_r) = in_r;
    }
    if (in_i < (stats->min_i)) {
        (stats->min_i) = in_i;
    }
    return 0;
}

static inline int print_analysis_report(Statistics_S *correct_stats, Statistics_S *wrong_stats) {
    // Print the analysis report
    printf("Analysis report\n");
    printf("Total count: %d\n", correct_stats->count + wrong_stats->count);
    printf("**********CORRECT**********\n");
    printf("Correct count: %d\n", correct_stats->count);
    printf("Correct average (r): %lf\n", correct_stats->sum_r/correct_stats->count);
    printf("Correct average (i): %lf\n", correct_stats->sum_i/correct_stats->count);
    printf("Correct max (r): %lf\n", correct_stats->max_r);
    printf("Correct max (i): %lf\n", correct_stats->max_i);
    printf("Correct min (r): %lf\n", correct_stats->min_r);
    printf("Correct min (i): %lf\n", correct_stats->min_i);
    printf("**********WRONG**********\n");
    printf("Wrong count: %d\n", wrong_stats->count);
    printf("Wrong average (r): %lf\n", wrong_stats->sum_r/wrong_stats->count);
    printf("Wrong average (i): %lf\n", wrong_stats->sum_i/wrong_stats->count);
    printf("Wrong max (r): %lf\n", wrong_stats->max_r);
    printf("Wrong max (i): %lf\n", wrong_stats->max_i);
    printf("Wrong min (r): %lf\n", wrong_stats->min_r);
    printf("Wrong min (i): %lf\n", wrong_stats->min_i);
    return 0;
}

int main(void);


#endif // MAIN_H 