#include "Matrix.h"
#include "gauss.h"

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <functional>

using namespace std;

const int N = 20;

double freq[N] = {2.160913, 2.184642, 2.208656, 2.232956, 2.257543, 2.282417, 2.307579, 2.333029,
                  2.358767, 2.384794, 2.411110, 2.437714, 2.464608, 2.491789, 2.519259, 2.547017, 2.575062,
                  2.603393, 2.632010, 2.660913};

double s21[5][N] = {
        0.0154473160, 0.0182357086, 0.0194462133, 0.0118513804, 0.0414972492, 0.4124997372,
        0.9972000658, 0.9942401537, 0.9938543943, 0.9845163563, 0.9975239226, 0.9947690590, 0.9844258408,
        0.9994965668, 0.8073331021, 0.4250811874, 0.2196173998, 0.1257758825, 0.0785875430, 0.0524021994
};


vector<Equation> genEquations(const GameDesc &gd) {
    vector<X> xs;
    vector<Equation> eqs;
    const auto &traps = gd.traps;

    xs.push_back(X {1, 0, 0});

    for (int i = 0; i < xs.size(); ++i) {
        X x = xs[i];

        int c = x.g == 1 ? x.a : x.b;

        Equation eq;
        eq.x = x;

        for (int i = 1; i <= D; ++i) {
            int cn = c + i;

            auto trapIt = find_if(traps.begin(), traps.end(), [=](Trap t) {
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

                auto eIt = find_if(eq.exprs.begin(), eq.exprs.end(), [=](Expr e) {
                    return e.x == xn;
                });
                if (eIt != eq.exprs.end()) {
                    Expr &expr = *eIt;
                    expr.a += 1. / D;
                } else {
                    eq.exprs.push_back({1. / D, xn});
                }

                auto xsIt = find(xs.begin(), xs.end(), xn);
                if (xsIt == xs.end()) {
                    xs.push_back(xn);
                }
            }
        }

        eqs.push_back(eq);
    }

    return eqs;
}

pair<Matrix, Matrix> genSystem(const vector<Equation> &eqs) {
    int m = eqs.size();
    int n = m;

    map<X, int> x2j;
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

vector<double> coefs() {

}

double dj(int j, const vector<double> & X, const vector<double> & c) {
    return (cj(j + 1, c) - cj(j, c)) / (3 * hj(j, X));
}

double bj(int j, const vector<double> & X, const vector<double> & Y, const vector<double> & c) {
    double _hj = hj(j, X);
    double _cj = cj(j, c);
    return dyj(j, Y) / _hj - _cj * _hj - dj(j, X, c) * _hj * _hj;
}

double Sj(int j, double x, const vector<double> & X, const vector<double> & Y, const vector<double> & c) {
    double _dx = x - xj(X, j);
    return yj(Y, j) + bj(j, c) * _dx + cj(j, c) * _dx * _dx + dj(j, c) * _dx * _dx * _dx;
}

double S(double x, const vector<double> & X, const vector<double> & Y, const vector<double> & c) {
    assert(X.size() > 0 && X.size() == Y.size());
    int n = X.size();

    double x1 = X.front();
    double xn = X.back();
    assert(x >= x1 && x <= xn);

    for (int j = 1; j < n; ++j) {
        if (x < xj(j, X)) {
            return Sj(j, X, Y, c);
        }
    }
}

vector<double> interpolate(const vector<double> & X, const vector<double> & Y, int N) {
    vector<double> c = coefs();

    double x1 = X.front();
    double xn = X.back();

    vector<double> Yi(N);
    for (int i = 0; i < N; ++i) {
        double x = x1 + (xn - x1) / N * i;
        Yi[i] = S(x, c);
    }
}

int main(int argc, const char *argv[]) {
    int n, N;
    cin >> n >> N;

    vector<double> X(n), Y(n);
    for (auto &x : X) {
        cin >> x;
    }
    for (auto &y : Y) {
        cin >> y;
    }

    vector<double> Yi = interpolate(X, Y, N);
}

