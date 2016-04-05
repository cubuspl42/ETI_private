#include <assert.h>
#include "gauss.h"

Matrix gauss(const Matrix & a, const Matrix & b) {
    Matrix ab = a.extend(b);

    int m = ab.numRows();
    int n = ab.numCols();

    for (int i = 1; i <= m; ++i) {
        ab.scaleRow(i, 1./ ab.get(i, i));
        for (int k = 1; k <= m; ++k) {
            if (k != i) {
                double aki = ab.get(k, i);
                ab.combineRows(k, i, -aki);
            }
        }
    }

    return ab.extractCol(ab.numCols());
}

Matrix gaussSeidel(const Matrix & a, const Matrix & b, int ni) {
    assert(a.numRows() == a.numCols());
    assert(a.numRows() == b.numRows());
    assert(b.numCols() == 1);

    int m = a.numRows();
    int n = a.numCols();

    Matrix xk(m, 1);

    for (int i = 1; i <= m; ++i) {
        xk.get(i, 1) = 0.5;
    }

    for (int k = 1; k <= ni; ++k) {
        Matrix xk1(m, 1);

        for (int i = 1; i <= m; ++i) {
            double bi = b.get(i, 1);

            double s1 = 0;
            for (int j = 1; j < i; ++j) {
                s1 += a.get(i, j) * xk1.get(j, 1);
            }

            double s2 = 0;
            for (int j = i + 1; j <= n; ++j) {
                s2 += a.get(i, j) * xk.get(j, 1);
            }

            double aii = a.get(i, i);
            xk1.get(i, 1) = (bi - s1 - s2) / aii;
        }

        xk = xk1;
    }

    return xk;
}