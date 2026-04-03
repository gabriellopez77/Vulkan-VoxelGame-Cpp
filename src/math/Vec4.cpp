#include "Vec4.h"

#include <cmath>


Vec4 Vec4::lerp(const Vec4& a, const Vec4& b, float t) {
    return {
        (t * (b.x - a.x)) + a.x,
        (t * (b.y - a.y)) + a.y,
        (t * (b.z - a.z)) + a.z,
        (t * (b.w - a.w)) + a.w,
    };
}

Vec4 Vec4::normalize(const Vec4& vec) {
    return vec.normalized();
}

float Vec4::dot(const Vec4& left, const Vec4& right) {
    const Vec4 temp(left * right);

    return temp.x + temp.y + temp.z + temp.w;
}


Vec4 Vec4::normalized() const {
    Vec4 temp = *this;

    return temp * 1 / std::sqrt(dot(temp, temp));
}

float Vec4::length() const {
    return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

void Vec4::normalize() {
    *this = normalized();
}