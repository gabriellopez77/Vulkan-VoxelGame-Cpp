#include "Vec2.h"

#include <cmath>


Vec2 Vec2::normalize(const Vec2& vec) {
    return vec.normalized();
}

Vec2 Vec2::lerp(const Vec2& a, const Vec2& b, float t) {
    return {
        (t * (b.x - a.x)) + a.x,
        (t * (b.y - a.y)) + a.y
    };
}

float Vec2::dot(const Vec2& left, const Vec2& right) {
    const Vec2 temp(left * right);

    return temp.x + temp.y;
}


Vec2 Vec2::normalized() const {
    Vec2 temp = *this;

    return temp * 1 / std::sqrt(dot(temp, temp));
}

float Vec2::length() const {
    return std::sqrt((x * x) + (y * y));
}

void Vec2::normalize() {
    *this = normalized();
}