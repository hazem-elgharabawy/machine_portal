#include "../inc/actuator.h"
extern inline fixed_point_t fixed_mul(fixed_point_t a, fixed_point_t b);  // Input real part
/*
----------------------------------------------------------------
actuator general equation
Y = H(1)*X + H(2)*X*|X|^2 + H(3)*X*|X|^4
    
    H(1) = a10
    H(2) = a30
    H(3) = a50

----------------------------------------------------------------
*/

// Single-core implementation
void actuator_func(Actuator_S *act) {

    // Compute |X|^2 (input magnitude squared)
    fixed_point_t abs_squared = fixed_mul(act->in_r, act->in_r) +
                                fixed_mul(act->in_i, act->in_i);

    // Compute |X|^4 (input magnitude to the power of 4)
    fixed_point_t abs_pwr_four = fixed_mul(abs_squared, abs_squared);

    // Compute intermediate results for cubic and quintic terms
    fixed_point_t half_mul1_r = fixed_mul(abs_squared, A30_R);
    fixed_point_t half_mul1_i = fixed_mul(abs_squared, A30_I);
    fixed_point_t half_mul2_r = fixed_mul(abs_pwr_four, A50_R);
    fixed_point_t half_mul2_i = fixed_mul(abs_pwr_four, A50_I);

    // Combine intermediate results
    fixed_point_t add1_r = half_mul1_r + A10_R;
    fixed_point_t add1_i = half_mul1_i + A10_I;
    fixed_point_t add2_r = half_mul2_r + add1_r;
    fixed_point_t add2_i = half_mul2_i + add1_i;

    // Compute final outputs
    fixed_point_t mul1 = fixed_mul(add2_r, act->in_r);
    fixed_point_t mul2 = fixed_mul(add2_i, act->in_i);
    fixed_point_t mul3 = fixed_mul(add2_r, act->in_i);
    fixed_point_t mul4 = fixed_mul(add2_i, act->in_r);

    act->out_r = mul1 - mul2;
    act->out_i = mul3 + mul4;
}
