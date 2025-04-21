
#ifndef THREADS_FIXED_POINT_H   //header gaurd recommended so that file is not included more than once
#define THREADS_FIXED_POINT_H
#define F 16384  // 2^14 
#include <stdint.h>    // ((int64_t)x) * y / F    prevent overflow here .

typedef struct real{
int value;
} real;

    real Convert_n_to_fixed_point(int n);
    
    int Convert_x_to_integer_round_zero(real x);
    int Convert_x_to_integer_round_nearest(real x);
    
    real Add_x_and_y(real x, real y);
    
    real Subtract_y_from_x(real x, real y);

    real Multiply_x_by_y(real x, real y);

    real Divide_x_by_y(real x, real y);
    // mixed 
    real Add_x_and_n(real x, int n);
    
    real Subtract_n_from_x(real x, int n);
    
    real Multiply_x_by_n(real x, int n);
    
    real Divide_x_by_n(real x, int n);

    
    
#endif
