#include "Vec3.h"

#include <cmath>


Vec3 Vec3::cross(const Vec3& left, const Vec3& right) {
    return {
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x
    };
}

Vec3 Vec3::normalize(const Vec3& vec) {
    return vec.normalized();
}

Vec3 Vec3::lerp(const Vec3& a, const Vec3& b, float t) {
    return {
        (t * (b.x - a.x)) + a.x,
        (t * (b.y - a.y)) + a.y,
        (t * (b.z - a.z)) + a.z
    };
}

float Vec3::dot(const Vec3& left, const Vec3& right) {
    const Vec3 temp(left * right);

    return temp.x + temp.y + temp.z;
}


Vec3 Vec3::normalized() const {
    Vec3 temp = *this;

    return temp * 1 / std::sqrt(dot(temp, temp));
}

float Vec3::length() const {
    return std::sqrt((x * x) + (y * y) + (z * z));
}

void Vec3::normalize() {
    *this = normalized();
}