#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <functional>
#include <cassert>
#include <fstream>
#include <iomanip>

using namespace std;

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

    void print() const;
};

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

void Matrix::print() const {
    int m = numRows();
    int n = numCols();

    std::cerr.precision(6);

    for (int i = 1; i <= m; ++i) {
        std::cerr << "[ ";
        for (int j = 1; j <= n; ++j) {
            std::cerr << std::setw(6) << get(i, j) << " ";
        }
        std::cerr << "]" << std::endl;
    }

    std::cerr << std::endl;
}


Matrix gauss(const Matrix & a, const Matrix & b) {
    Matrix ab = a.extend(b);

    int m = ab.numRows();

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
