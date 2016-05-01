#include "Matrix.h"
#include "gauss.h"

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <functional>

const int N = 20;

double freq[N] = {2.160913, 2.184642, 2.208656, 2.232956, 2.257543, 2.282417, 2.307579, 2.333029,
                  2.358767, 2.384794, 2.411110, 2.437714, 2.464608, 2.491789, 2.519259, 2.547017, 2.575062,
                  2.603393, 2.632010, 2.660913};

double s21[5][N] = {
        0.0154473160, 0.0182357086, 0.0194462133, 0.0118513804, 0.0414972492, 0.4124997372,
        0.9972000658, 0.9942401537, 0.9938543943, 0.9845163563, 0.9975239226, 0.9947690590, 0.9844258408,
        0.9994965668, 0.8073331021, 0.4250811874, 0.2196173998, 0.1257758825, 0.0785875430, 0.0524021994
};

const int D = 6;

struct X {
    int g = 1;
    int a = 0;
    int b = 0;

    X() = default;

    X(int g, int a, int b) : g(g), a(a), b(b) { }

    bool operator<(const X &o) const {
        return std::make_tuple(g, a, b) < std::make_tuple(o.g, o.a, o.b);
    }

    bool operator==(const X &o) const {
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
    Expr(double a, X x) : a(a), x(x) { }

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
    GameDesc(int n, std::vector<Trap> traps) : n(n), traps(traps) { }

    int n = 1;
    std::vector<Trap> traps;
};

std::vector<double> interpolate(std::vector<double> vector, std::vector<double> y, int n);

std::vector<Equation> genEquations(const GameDesc &gd) {
    std::vector<X> xs;
    std::vector<Equation> eqs;
    const auto &traps = gd.traps;

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
            if (trapIt != traps.end()) {
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
                if (xsIt == xs.end()) {
                    xs.push_back(xn);
                }
            }
        }

        eqs.push_back(eq);
    }

    return eqs;
}

std::pair<Matrix, Matrix> genSystem(const std::vector<Equation> &eqs) {
    int m = eqs.size();
    int n = m;

    std::map<X, int> x2j;
    int j = 1;
    for (auto &eq : eqs) {
        x2j[eq.x] = j;
        ++j;
    }

    Matrix a(m, n);
    Matrix b(m, 1);

    int i = 1;
    for (auto &eq : eqs) {
        int xj = x2j[eq.x];
        a.get(i, xj) = -1;

        for (auto &ex : eq.exprs) {
            int j = x2j[ex.x];
            a.get(i, j) = ex.a;
        }

        b.get(i, 1) = -eq.d;
        ++i;
    }

    return {a, b};
}

std::vector<double> coefs() {

}

double Si() {

}

double S(double x, const std::vector<double> & c) {

}

std::vector<double> interpolate(const std::vector<double> & X, const std::vector<double> & Y, int N) {
    std::vector<double> c = coefs();

    double x1 = X.front();
    double xn = X.back();

    std::vector<double> Yi(N);
    for (int i = 0; i < N; ++i) {
        double x = x1 + (xn - x1) / N * i;
        Yi[i] = S(x, c);
    }
}

int main(int argc, const char *argv[]) {
    int n, N;
    std::cin >> n >> N;

    std::vector<double> X(n), Y(n);
    for (auto &x : X) {
        std::cin >> x;
    }
    for (auto &y : Y) {
        std::cin >> y;
    }

    std::vector<double> Yi = interpolate(X, Y, N);
}

