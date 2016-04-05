#ifndef MN2_GAUSS_H
#define MN2_GAUSS_H


#include "Matrix.h"

Matrix gauss(const Matrix & a, const Matrix & b);

Matrix gaussSeidel(const Matrix & a, const Matrix & b, int ni);


#endif //MN2_GAUSS_H
