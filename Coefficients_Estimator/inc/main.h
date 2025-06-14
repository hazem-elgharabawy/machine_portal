#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "pulp.h"
#include "fixed.h"
#include "actuator.h"


#define TEST
#ifdef TEST
#include  "post_dist_out_i.h"
#include "post_dist_out_r.h"
#include "sync_pa_in_i.h"
#include "sync_pa_in_r.h"
#include "sync_pa_out_i.h"
#include "sync_pa_out_r.h"
#include "y_h_e_1_i.h"
#include "y_h_e_1_r.h"
#include "y_h_e_2_i.h"  
#include "y_h_e_2_r.h"
#include "y_h_e_3_i.h"
#include "y_h_e_3_r.h"
#include "y_h_y_11.h"
#include "y_h_y_12.h"
#include "y_h_y_13.h"   
#include "y_h_y_23.h"
#include "y_h_y_33.h"
#define MARGIN 4
#endif // TEST


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