//
//  main.cpp
//  PO
//
//  Created by Jakub Trzebiatowski on 04/03/15.
//  Copyright (c) 2015 Jakub Trzebiatowski. All rights reserved.
//

#include <iostream>
#include <fstream>

using namespace std;

template<typename T>
class Vector {
    T *_data;
    size_t _size;
public:
    Vector() : _data(nullptr), _size(0) {
        
    }
    
    Vector(const Vector &other) {
        resize(other.size());
        for(size_t i = 0; i < _size; ++i) {
            _data[i] = other[i];
        }
    }
    
    Vector &operator=(const Vector &other) {
        Vector tmp(other);
        std::swap(_data, tmp._data);
        std::swap(_size, tmp._size);
    }
    
    size_t size() const {
        return _size;
    }
    
    void resize(size_t newsize) {
        delete[] _data;
        _data = new T[newsize];
        _size = newsize;
    }
    
    inline T &operator[](size_t index) {
        return _data[index];
    }
    
    inline const T &operator[](size_t index) const {
        return _data[index];
    }
    
    void write(ostream &os) const {
        const char *n = _size > 3 ? "\n" : " ";
        os << "Vector<" << typeid(T).name() << ">[" << size() << "] {" << n;
        for(size_t i = 0; i < _size; ++i) {
            os << _data[i] << "," << n;
        }
        os << "}";
    }
    
    void read(istream &is) {
        size_t size;
        is >> size;
        resize(size);
        for(size_t i = 0; i < size; ++i) {
            is >> _data[i];
        }
    }
};

template<typename T>
ostream &operator <<(ostream &os, const Vector<T> &vector) {
    vector.write(os);
    return os;
}

template<typename T>
istream &operator >>(istream &is, Vector<T> &vector) {
    vector.read(is);
    return is;
}

typedef Vector<double> VectorNd;
typedef Vector<VectorNd> VectorOfVectorsNd;

void wypiszV1(VectorNd v){
    cout << "|----------BEGIN------------------------------|"<<endl;
    cout << "Jestem wektorem pod adresem: " << &v <<endl;
    size_t wymiar = v.size();
    cout << "Mój wymiar to: " << wymiar << endl;
    cout << "A elementy:"<<endl;
    for(size_t i = 0; i < wymiar; ++i)
        cout << "[" << i << "] = \t" << v[i] << endl;
    cout << "|-----------END-------------------------------|"<<endl;
}

void wypiszV2(VectorNd& v){
    cout << "|----------BEGIN------------------------------|"<<endl;
    cout << "Jestem wektorem pod adresem: " << &v <<endl;
    size_t wymiar = v.size();
    cout << "Mój wymiar to: " << wymiar << endl;
    cout << "A elementy:"<<endl;
    for(size_t i = 0; i < wymiar; ++i)
        cout << "[" << i << "] = \t" << v[i] << endl;
    cout << "|-----------END-------------------------------|"<<endl;
}

int main() {
    VectorOfVectorsNd vof;
    cin >> vof;
    
    cout << "Adres wektora: " << &vof << endl << endl;
    
    wypiszV1(vof[0]);
    cout << endl;
    
    wypiszV2(vof[0]);
    cout << endl;
    
    cout << vof << endl;
    ofstream of;
    
    of.open("out");
    of << vof;
    
    return 0;
}
