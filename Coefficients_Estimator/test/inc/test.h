#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "../../inc/fixed.h"
#include "../../inc/mat_inv.h"
#include "../../inc/input_data.h"
#include "../../inc/output_data.h"

// Number of test matrices
#define NUM_TEST_MATRICES NUM_INPUT_MATRICES

// Use the arrays from input_data.h and output_data.h
#define input input_data
#define output output_data

#endif // TEST_H 