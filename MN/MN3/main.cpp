#include "Matrix.h"
#include "gauss.h"

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <functional>
#include <cassert>
#include <fstream>

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


double xj(int j, const vector<double> & X) {
    return X[j - 1];
}

double hj(int j, const vector<double> & X) {
    return xj(j + 1, X) - xj(j, X);
}

double yj(int j, const vector<double> & Y) {
    return Y[j - 1];
}

double dyj(int j, const vector<double> & Y) {
    return yj(j + 1, Y) - yj(j, Y);
}

double cj(int j, const vector<double> & c) {
    return c[j - 1];
}

pair<Matrix, Matrix> gensys(const vector<double> & X, const vector<double> & Y) {
    assert(X.size() > 0 && X.size() == Y.size());
    int n = X.size();

    Matrix a(n, n);
    Matrix b(n, 1);

    a.get(1, 1) = a.get(n, n) = 1;

    for (int i = 2, j = 1; i <= n - 1; ++i, ++j) {
        assert(j >= 1 && j <= n - 2);
        double _h1 = hj(j, X);
        double _h2 = hj(j + 1, X);
        a.get(i, j) = _h1;
        a.get(i, j + 1) = 2 * (_h1 + _h2);
        a.get(i, j + 2) = _h2;

        b.get(i, 1) = (3 * dyj(j + 1, Y) / _h2) - (3 * dyj(j, Y) / _h1);
    }

    return make_pair(a, b);
}

vector<double> coefs(const vector<double> & X, const vector<double> & Y) {
    assert(X.size() > 0 && X.size() == Y.size());
    int n = X.size();

    auto p = gensys(X, Y);
    const Matrix & a = p.first;
    const Matrix & b = p.second;

    a.print();
    b.print();

    Matrix cm = gauss(a, b);

    cm.print();

    vector<double> c(n);
    for (int i = 0; i < cm.numRows(); ++i) {
        c[i] = cm.get(i + 1, 1);
    }

    return c;
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
    double _dx = x - xj(j, X);
    return yj(j, Y) + bj(j, X, Y, c) * _dx + cj(j, c) * _dx * _dx + dj(j, X, c) * _dx * _dx * _dx;
}

double S(double x, const vector<double> & X, const vector<double> & Y, const vector<double> & c) {
    assert(X.size() > 0 && X.size() == Y.size());
    int n = X.size();

    double x1 = X.front();
    double xn = X.back();
    assert(x >= x1 && x <= xn);

    for (int j = 1; j <= n - 1; ++j) {
        if (x <= xj(j + 1, X)) {
            return Sj(j, x, X, Y, c);
        }
    }

    return -1;
}

pair<vector<double>, vector<double>> interpolate(const vector<double> & X, const vector<double> & Y, int N) {
    assert(X.size() > 0 && X.size() == Y.size());

    cout.precision(numeric_limits<double>::max_digits10);

    vector<double> c = coefs(X, Y);

    double x1 = X.front();
    double xn = X.back();

    vector<double> Xi(N);
    vector<double> Yi(N);
    for (int i = 0; i < N; ++i) {
        double x = x1 + (xn - x1) * i / (N - 1);
        Xi[i] = x;
        Yi[i] = S(x, X, Y, c);
    }

    return make_pair(Xi, Yi);
}

void dump_list(const vector<double> & v) {
    cout << "[" << endl;
    for (int i = 0; i < v.size(); ++i) {
        cout << v[i];
        if (i < v.size() - 1) {
            cout << ",";
        }
        cout << endl;
    }
    cout << "]" << endl;
}

void mn3(istream & is) {
    int n, N;
    is >> n >> N;

    assert(n > 0 && N > n);

    vector<double> X(n), Y(n);
    for (auto &x : X) {
        is >> x;
    }
    for (auto &y : Y) {
        is >> y;
    }

    auto p = interpolate(X, Y, N);
    const vector<double> & Xi = p.first;
    const vector<double> & Yi = p.second;

    cout << "[" << endl;
    dump_list(Xi);
    cout << "," << endl;
    dump_list(Yi);
    cout << "]" << endl;

}

int main(int argc, const char *argv[]) {
    if (argc > 1) {
        ifstream is;
        is.open(argv[1]);
        mn3(is);
    } else {
        mn3(cin);
    }
    return 0;
}

