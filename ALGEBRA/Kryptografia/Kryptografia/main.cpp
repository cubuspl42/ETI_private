//
//  main.cpp
//  Kryptografia
//
//  Created by Jakub Trzebiatowski on 26/10/14.
//  Copyright (c) 2014 Jakub Trzebiatowski. All rights reserved.
//

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

template<typename T>
class Matrix {
public:
    Matrix(size_t m, size_t n_) : fields(m*n_, 0) {
        n = n_;
    }
    Matrix(std::initializer_list<std::initializer_list<T>> rows) {
        size_t m = rows.size();
        n = rows.begin()->size();
        fields.reserve(m*n);
        for(auto &row : rows) {
            assert(row.size() == n);
            for(auto element : row) {
                fields.push_back(element);
            }
        }
    }
    size_t rows() const {
        return fields.size()/n;
    };
    size_t columns() const {
        return n;
    };
    T* operator[](size_t i) {
        return fields.data() + i*n;
    }
    const T* operator[](size_t i) const {
        return fields.data() + i*n;
    }
    Matrix operator*(const Matrix &b) const {
        const Matrix &a = *this;
        assert(a.columns() == b.rows());
        size_t m = a.rows(), n = b.columns(), r = a.columns();
        Matrix c(m, n);
        for(size_t i = 0; i < m; ++i) {
            for(size_t j = 0; j < n; ++j) {
                for(size_t k = 0; k < r; ++k) {
                    c[i][j] += a[i][k] * b[k][j];
                }
            }
        }
        return c;
    }
private:
    size_t n = 0;
    std::vector<T> fields;
    template<typename T2>
    friend std::istream& operator>>(std::istream &, Matrix<T2> &);
    template<typename T2>
    friend std::ostream& operator<<(std::ostream &, const Matrix<T2> &);
};

void inverse_matrix_3x3(Matrix<int> &matrix) {
    
}

template<typename T>
inline std::ostream& operator<<(std::ostream &os, const Matrix<T> &matrix) {
    size_t m = matrix.rows(), n = matrix.columns();
    for(size_t i = 0; i < m; ++i) {
        for(size_t j = 0; j < n; ++j) {
            os << matrix[i][j] << ((j == n-1) ? '\n' : ' ');
        }
    }
    return os;
}

template<typename T>
inline std::istream& operator>>(std::istream &is, Matrix<T> &matrix) {
    size_t m = matrix.rows(), n = matrix.columns();
    for(size_t i = 0; i < m; ++i) {
        for(size_t j = 0; j < n; ++j) {
            is >> matrix[i][j];
        }
    }
    return is;
}

Matrix<int> read_matrix(const char *filename) {
    std::ifstream is(filename);
    size_t m, n;
    is >> m >> n;
    Matrix<int> matrix(m, n);
    is >> matrix;
    return matrix;
}

int main(int argc, const char * argv[]) {
    if(argc != 4) {
        std::cerr << "USAGE: crypto <command> <message_file> <key_file>\n";
        return 1;
    }
    bool encode = false;
    if(!strcmp(argv[1], "encode")) {
        encode = true;
    } else if(strcmp(argv[1], "decode")) {
        std::cerr << "<command> should be either encode or decode\n";
        return 1;
    }
    Matrix<int> key = read_matrix(argv[3]);
    if(encode) {
        std::ifstream is(argv[2]);
        std::string message((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        size_t m = key.columns(), n = message.length()/m + 1;
        Matrix<int> message_matrix(m, n);
        for(size_t k=0; k<message.length(); ++k) {
            size_t i = k%m, j = k/m;
            message_matrix[i][j] = (int)message[k];
        }
        std::cout << message_matrix << std::endl;
        std::cout << key << std::endl;
        std::cout << key*message_matrix << std::endl;
    } else {
        Matrix<int> encoded = read_matrix(argv[2]);
    }
    return 0;
}
