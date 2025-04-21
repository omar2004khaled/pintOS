
#include "fixed_point.h"
#include <stdint.h>
// n aand m are integer          while               x and y are fixed points 

real Convert_n_to_fixed_point(int n){
real result;
result.value=    n * F;
return result;
}

int Convert_x_to_integer_round_zero(real x){
return    x.value / F;
}

int Convert_x_to_integer_round_nearest(real x){
if(x.value>=0) return (x.value + F / 2) / F ;
else return (x.value - F / 2) / F ;
}
/****************************/

real Add_x_and_y(real x, real y){
real result;
result.value=x.value+y.value;
return    result;
}

real Subtract_y_from_x(real x, real y){
    real result;
    result.value=x.value-y.value;
    return    result;
}
real Multiply_x_by_y(real x, real y){
    real result;   
    result.value=  ((int64_t) x.value) * y.value / F;
    return result;
}
real Divide_x_by_y(real x, real y) {
    if (y.value == 0) {
        real result = {0}; // Initialize a `real` struct with 0
        return result;
    }
    real result;   
    result.value=(((int64_t)x.value * F) / y.value);
    return result;
}
/******************************/
// mixed 
real Add_x_and_n(real x, int n){
    real result;   
    result.value =  x.value + n * F;
    return result;
}

real Subtract_n_from_x(real x, int n){
    real result;   
    result.value =  x.value - n * F;
    return result;
}

real Multiply_x_by_n(real x, int n){
    real result;   
    result.value =   x.value * n;
    return result;
}

real Divide_x_by_n(real x, int n){
    real result;   
    result.value =     x.value/ n;
    return result;
}



/*
int Add_m_and_n(int m, int n) {
    return Convert_n_to_fixed_point(m) + Convert_n_to_fixed_point(n);
}
int Subtract_n_from_m(int m, int n) {
    return Convert_n_to_fixed_point(m) - Convert_n_to_fixed_point(n);
}
int Multiply_m_by_n(int m, int n){
    return    m * n;
    };
    int Divide_m_by_n(int m , int n){
    m = Convert_n_to_fixed_point(m) ;
    n = Convert_n_to_fixed_point(n) ;
    return Divide_x_by_y(m,n);
};
*/