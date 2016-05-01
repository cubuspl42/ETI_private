//
// Created by kuba on 05.04.16.
//

#ifndef MN2_MATRIX_H
#define MN2_MATRIX_H


#include <vector>

class Matrix {

private:

    std::vector<std::vector<double>> _rows;

public:

    Matrix(int m, int n);

    int numRows() const;

    int numCols() const;

    double & get(int i, int j);

    double get(int i, int j) const;

    void scaleRow(int i, double s);

    void combineRows(int i, int k, double s);

    Matrix extend(const Matrix & o) const;

    Matrix extractCol(int j) const;

    void print();

};


#endif //MN2_MATRIX_H
