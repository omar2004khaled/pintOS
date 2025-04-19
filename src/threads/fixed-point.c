#include "fixed-point.h"

void init_real(real* r, int n) {   // Initialize real with an integer value
    r->value = Convert_n_to_fixed_point(n);
} // n aand m are integer  while x and y are fixed points 
int Convert_n_to_fixed_point(int n){
return    n * f;
};
int Convert_x_to_integer_round_zero(int x){
return    x / f;
};
int Convert_x_to_integer_round_nearest(int x){
if(x>=0) return (x + f / 2) / f ;
else return (x - f / 2) / f ;
};
int Add_x_and_y(int x, int y){
return    x + y;
};
int Subtract_y_from_x(int x, int y){
return    x - y;
};
int Add_m_and_n(int m, int n) {
    return Convert_n_to_fixed_point(m) + Convert_n_to_fixed_point(n);
};


int Subtract_n_from_m(int m, int n) {
    return Convert_n_to_fixed_point(m) - Convert_n_to_fixed_point(n);
};

int Add_x_and_n(int x, int n){
return    x + n * f;
};
int Subtract_n_from_x(int x, int n){
return    x - n * f;
};
int Multiply_x_by_y(int x, int y){
return  (int32_t)((int64_t) x) * y / f;
};
int Multiply_x_by_n(int x, int n){
return    x * n;
};
int Multiply_m_by_n(int m, int n){
    return    m * n;
    };
int Divide_x_by_y(int x, int y){
return   (int32_t)((int64_t) x) * f / y;
};
int Divide_x_by_n(int x, int n){
return    x / n;
};
int Divide_m_by_n(int m , int n){
    m = Convert_n_to_fixed_point(m) ;
    n = Convert_n_to_fixed_point(n) ;
    return Divide_x_by_y(m,n);
};