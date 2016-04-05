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

#include <assert.h>
#include <iostream>
#include <iomanip>

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

#ifndef MN2_GAUSS_H
#define MN2_GAUSS_H


Matrix gauss(const Matrix & a, const Matrix & b);

Matrix gaussSeidel(const Matrix & a, const Matrix & b, int ni);


#endif //MN2_GAUSS_H
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

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <functional>


const int D = 6;

struct X {
    int g = 1;
    int a = 0;
    int b = 0;

    X() = default;

    X(int g, int a, int b) : g(g), a(a), b(b) {}

    bool operator<(const X & o) const {
        return std::make_tuple(g, a, b) < std::make_tuple(o.g, o.a, o.b);
    }

    bool operator==(const X & o) const {
        return g == o.g && a == o.a && b == o.b;
    }
};

X _xn(X x, int c) {
    if (x.g == 1) {
        return X {2, c, x.b};
    } else {
        return X {1, x.a, c};
    }
}

struct Expr {
    Expr(double a, X x) : a(a), x(x) {}

    double a = 1;
    X x;
};

struct Equation {
    Equation() = default;

    X x;
    std::vector<Expr> exprs;
    double d = 0;
};

struct Trap {
    int i = 0;
    int k = -1;
};

struct GameDesc {
    GameDesc(int n, std::vector<Trap> traps) : n(n), traps(traps) {}

    int n = 1;
    std::vector<Trap> traps;
};

std::vector<Equation> genEquations(const GameDesc & gd) {
    std::vector<X> xs;
    std::vector<Equation> eqs;
    const auto & traps = gd.traps;

    xs.push_back(X {1, 0, 0});

    for (int i = 0; i < xs.size(); ++i) {
        X x = xs[i];

        int c = x.g == 1 ? x.a : x.b;

        Equation eq;
        eq.x = x;

        for (int i = 1; i <= D; ++i) {
            int cn = c + i;

            auto trapIt = std::find_if(traps.begin(), traps.end(), [=](Trap t) {
                return t.i == cn;
            });
            if(trapIt != traps.end()) {
                Trap t = *trapIt;
                cn += t.k;
            }

            if (cn >= gd.n) {
                if (x.g == 1) {
                    eq.d += 1. / D;
                }
            } else {
                X xn = _xn(x, cn);

                auto eIt = std::find_if(eq.exprs.begin(), eq.exprs.end(), [=](Expr e) {
                    return e.x == xn;
                });
                if (eIt != eq.exprs.end()) {
                    Expr &expr = *eIt;
                    expr.a += 1. / D;
                } else {
                    eq.exprs.push_back({1. / D, xn});
                }

                auto xsIt = std::find(xs.begin(), xs.end(), xn);
                if(xsIt == xs.end()) {
                    xs.push_back(xn);
                }
            }
        }

        eqs.push_back(eq);
    }

    return eqs;
}

std::pair<Matrix, Matrix> genSystem(const std::vector<Equation> & eqs) {
    int m = eqs.size();
    int n = m;

    std::map<X, int> x2j;
    int j = 1;
    for(auto & eq : eqs) {
        x2j[eq.x] = j;
        ++j;
    }

    Matrix a(m, n);
    Matrix b(m, 1);

    int i = 1;
    for(auto & eq : eqs) {
        int xj = x2j[eq.x];
        a.get(i, xj) = -1;

        for (auto & ex : eq.exprs) {
            int j = x2j[ex.x];
            a.get(i, j) = ex.a;
        }

        b.get(i, 1) = -eq.d;
        ++i;
    }

    return {a, b};
}

int roll() {
    return (rand() % 6) + 1;
}

double monteCarlo(const GameDesc & gd, int n) {
    double nw1 = 0;

    for (int ni = 0; ni < n; ++ni) {
        X x {1, 0, 0};

        while (x.a < gd.n && x.b < gd.n) {
            int c = x.g == 1 ? x.a : x.b;
            c += roll();

            auto trapIt = std::find_if(gd.traps.begin(), gd.traps.end(), [=](Trap t) {
                return t.i == c;
            });
            if (trapIt != gd.traps.end()) {
                c += trapIt->k;
            }

            x = _xn(x, c);
        }

        if (x.a >= gd.n) {
            ++nw1;
        }
    }

    return nw1 / n;
}

auto p2 = GameDesc {6, {{4, -2}, {5, -2}}};

auto gd5 = GameDesc {28, {{1, -1}, {2, -2}, {3, -3}, {7, -1}, {13, -4}, {14, -5}, {17, -1}, {20, -1}, {21, -2}, {22, -3}, {24, -1}, {25, -2}, {26, -3}}};

auto gd9 = GameDesc {28, {{2, -1}, {4, -4}, {5, -2}, {6, -5}, {7, -6}, {10, -1}, {13, -2}, {15, -1}, {18, -4},{20, -9}, {22, -3}, {24, -1}, {26, -26}}};

auto gdf1 = GameDesc {27, {{4, -2}, {5, -2}, {7, -5}, {9, -3}, {14, -12}, {15, -2}, {17, -7}, {19, -8}, {21, -3}, {22, -16}, {25, -9}}};

int main(int argc, const char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " method n" << std::endl;
        return EXIT_FAILURE;
    }

    std::string method = argv[1];
    int n = std::atoi(argv[2]);

    const auto gd = gd9;

    std::map<std::string, std::function<double(int)>> f;

    f["gauss"] = [&](int _) {
        auto eqs = genEquations(gd);
        auto p = genSystem(eqs);
        auto a = p.first;
        auto b = p.second;
        auto g = gauss(a, b);
        return g.get(1, 1);
    };

    f["gauss-seidel"] = [&](int n) {
        auto eqs = genEquations(gd);
        auto p = genSystem(eqs);
        auto a = p.first;
        auto b = p.second;
        auto gs = gaussSeidel(a, b, n);
        return gs.get(1, 1);
    };

    f["montecarlo"] = [&](int n) {
        return monteCarlo(gd, n);
    };

    std::cout << f[method](n) << std::endl;
}