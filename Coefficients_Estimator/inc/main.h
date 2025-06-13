#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "pulp.h"
#include "fixed.h"
#include "actuator.h"

#define DATA_SIZE 6240
#define NUM_THREADS 8
#define NUM_ITERATIONS 1000

typedef struct {
    fixed_point_t yhe_1_r;               
    fixed_point_t yhe_1_i;               
    fixed_point_t yhe_2_r;              
    fixed_point_t yhe_2_i;              
    fixed_point_t yhe_3_r;              
    fixed_point_t yhe_3_i;              
} yhe_S;

typedef struct {
    fixed_point_t yhy_11;               
    fixed_point_t yhy_12;              
    fixed_point_t yhy_13;              
    fixed_point_t yhy_23;              
    fixed_point_t yhy_33;              
                 
} yhy_S;

int main(void);


#endif // MAIN_H 