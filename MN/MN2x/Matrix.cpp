//
// Created by kuba on 05.04.16.
//

#include <assert.h>
#include <iostream>
#include <iomanip>
#include "Matrix.h"

Matrix::Matrix(int m, int n) {
    _rows.resize(m);
    for (auto &r : _rows) {
        r.resize(n);
    }
}

void Matrix::combineRows(int i, int k, double s) {
    assert(i >= 1 && i <= numRows());
    assert(k >= 1 && k <= numRows());

    int n = numCols();
    for (int j = 1; j <= n; ++j) {
        get(i, j) += get(k, j) * s;
    }
}

void Matrix::scaleRow(int i, double s) {
    assert(i >= 1 && i <= numRows());

    for (auto &e : _rows[i - 1]) {
        e *= s;
    }
}

double & Matrix::get(int i, int j) {
    assert(i >= 1 && i <= numRows());
    assert(j >= 1 && j <= numCols());
    return _rows[i - 1][j - 1];
}

double Matrix::get(int i, int j) const {
    assert(i >= 1 && i <= numRows());
    assert(j >= 1 && j <= numCols());
    return _rows[i - 1][j - 1];
}

int Matrix::numCols() const {
    return _rows.empty() ? 0 : _rows.front().size();
}

int Matrix::numRows() const {
    return _rows.size();
}

Matrix Matrix::extend(const Matrix & o) const {
    assert(numRows() == o.numRows());

    int m = numRows();
    int n = numCols();
    int on = o.numCols();
    int en = n + on;
    Matrix e(m, en);

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            e.get(i, j) = get(i, j);
        }
    }

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= on; ++j) {
            e.get(i, n + j) = o.get(i, j);
        }
    }

    return e;
}

Matrix Matrix::extractCol(int j) const {
    int m = numRows();
    int n = numCols();

    Matrix c(numRows(), 1);

    for (int i = 1; i < m; ++i) {
        c.get(i, 1) = get(i, j);
    }

    return c;
}

void Matrix::print() {
    int m = numRows();
    int n = numCols();

    std::cout.precision(6);

    for (int i = 1; i <= m; ++i) {
        std::cout << "[ ";
        for (int j = 1; j <= n; ++j) {
            std::cout << std::setw(6) << get(i, j) << " ";
        }
        std::cout << "]" << std::endl;
    }

    std::cout << std::endl;
}
