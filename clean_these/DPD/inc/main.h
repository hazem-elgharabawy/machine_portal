#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include "pulp.h"
#include "time.h"


#include "actuator.h"
#include "fixed.h"
#include "input_i.h"
#include "input_r.h"

#define DATA_SIZE 6240
//#define PARALLEL // Uncomment to enable parallel processing
#ifdef PARALLEL
    #define NUM_THREADS 8
#endif
//#define PROFILE // Uncomment to enable profiling
#ifdef PROFILE
#include "perf_cnt.h"
#endif
#define TEST // Uncomment to enable testing
#ifdef TEST
#include "expected_i.h"
#include "expected_r.h"
#define MARGIN 4  
#endif

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
#endif