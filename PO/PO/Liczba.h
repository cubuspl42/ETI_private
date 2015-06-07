//
//  Liczba.h
//  PO
//
//  Created by Jakub Trzebiatowski on 04/03/15.
//  Copyright (c) 2015 Jakub Trzebiatowski. All rights reserved.
//

#ifndef PO_Liczba_h
#define PO_Liczba_h

#include <iostream>

class Wartosc_Liczbowa {
public: virtual double modul () = 0; //wartość bezwzględna
    virtual void wypisz(std::ostream &out) = 0;
    virtual ~Wartosc_Liczbowa() {}
};

class Liczba : public Wartosc_Liczbowa { double re; //czesc rzeczywista
public:
    Liczba operator+(Liczba & right) { return Liczba (this->re+right.re); }
    double modul();
    Liczba(double re);
    ~Liczba(void);
    friend std::ostream & operator<< (std::ostream &out, Liczba &l);
    void wypisz(std::ostream &out) { out<<*this; }
};

#endif
