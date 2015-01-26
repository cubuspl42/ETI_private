//
//  vector2.h
//  Arkanoid
//
//  Created by Jakub Trzebiatowski on 10/01/15.
//
//

#ifndef Arkanoid_vector2_h
#define Arkanoid_vector2_h

#include <cstddef>
#include <cmath>

struct Vector2
{
    float x;
    float y;
    Vector2(float x_=0, float y_=0)
    : x(x_), y(y_) {}
    Vector2& set(float x_, float y_) {
        x = x_;
        y = y_;
        return (*this);
    }
    float& operator [](size_t i) {
        static_assert((offsetof(Vector2, y)-offsetof(Vector2, x))==sizeof(float), "Offset mismatch");
        return ((&x)[i]);
    }
    float operator [](size_t i) const {
        return ((&x)[i]);
    }
    Vector2& operator +=(const Vector2& v) {
        x += v.x;
        y += v.y;
        return (*this);
    }
    Vector2& operator -=(const Vector2& v) {
        x -= v.x;
        y -= v.y;
        return (*this);
    }
    Vector2& operator *=(float t) {
        x *= t;
        y *= t;
        return (*this);
    }
    Vector2 operator -(void) const {
        return (Vector2(-x, -y));
    }
    Vector2 operator +(const Vector2& v) const {
        return (Vector2(x+v.x, y+v.y));
    }
    Vector2 operator -(const Vector2& v) const {
        return (Vector2(x-v.x, y-v.y));
    }
    Vector2 operator *(float t) const {
        return (Vector2(x*t, y*t));
    }
    Vector2 operator /(float t) const {
        return (Vector2(x*1.0f/t, y*1.0f/t));
    }
    float operator *(const Vector2& v) const {
        return (x*v.x+y*v.y);
    }
    bool operator ==(const Vector2& v) const {
        return ((x == v.x) && (y == v.y));
    }
    bool operator !=(const Vector2& v) const {
        return ((x != v.x) || (y != v.y));
    }
    float lengthsq() {
        return x*x+y*y;
    }
    float length() {
        return sqrt(lengthsq());
    }
    Vector2 normalized(void) {
        return (*this) / length();
    }
};

#endif
