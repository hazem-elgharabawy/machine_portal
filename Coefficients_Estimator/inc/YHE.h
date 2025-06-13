#include "../inc/main.h"

typedef struct {
    fixed_point_t yhe_1_r;               
    fixed_point_t yhe_1_i;               
    fixed_point_t yhe_2_r;              
    fixed_point_t yhe_2_i;              
    fixed_point_t yhe_3_r;              
    fixed_point_t yhe_3_i;              
} yhe_S;


int YHE_func(fixed_point_t *y_r, fixed_point_t *y_i, fixed_point_t *e_r, fixed_point_t *e_i, fixed_point_t *y_2, fixed_point_t *y_4);


