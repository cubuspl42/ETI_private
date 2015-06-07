//
//  Liczba.cpp
//  PO
//
//  Created by Jakub Trzebiatowski on 04/03/15.
//  Copyright (c) 2015 Jakub Trzebiatowski. All rights reserved.
//

#include "Liczba.h"

#include <iostream>
#include <cmath>

using namespace std;

Liczba::Liczba(double re) { cout << "Konstruktor Liczba = " << re << re; }
Liczba::~Liczba(void) { // TODO 1
    cout << *this;
}

double Liczba::modul() { //TODO 2
    return abs(re);
}

std::ostream & operator<<(std::ostream &out, Liczba &l) { out<<"Liczba rzeczywista: " << l.re<<"\n"; return out; }