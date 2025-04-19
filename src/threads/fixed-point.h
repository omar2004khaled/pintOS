#ifndef THREADS_FIXED_POINT_H   //header gaurd recommended so that file is not included more than once
#define THREADS_FIXED_POINT_H
#define f 16384  // 2^14 
#include <stdint.h>    // ((int64_t)x) * y / F    prevent overflow here .

typedef struct real{
int value;
} real;
void init_real(real* r, int n);
int Convert_n_to_fixed_point(int n);
int Convert_x_to_integer_round_zero(int x);
int Convert_x_to_integer_round_nearest(int x);


int Add_x_and_y(int x, int y);
int Subtract_y_from_x(int x, int y);
int Add_x_and_n(int x, int n);
int Add_m_and_n(int m, int n);
int Subtract_n_from_m(int m, int n);
int Subtract_n_from_x(int x, int n);
int Multiply_x_by_y(int x, int y);
int Multiply_x_by_n(int x, int n);
int Divide_x_by_y(int x, int y);
int Divide_x_by_n(int x, int n);
int Multiply_m_by_n(int m, int n);
int Divide_m_by_n(int m , int n);

#endif
