#pragma once

#include "Vec4.h"


// fwd
class Vec3;

class Matrix4 {
public:
    Matrix4(float value) {
        values[0] = { value, 0.f, 0.f, 0.f };
        values[1] = { 0.f, value, 0.f, 0.f };
        values[2] = { 0.f, 0.f, value, 0.f };
        values[3] = { 0.f, 0.f, 0.f, value };
    }

    static Matrix4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up);
    static Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far);
    static Matrix4 perspective(float fov, float aspect, float near, float far);

    Vec4 values[4];

    void translate(const Vec3& translation);
    void scale(const Vec3& scale);
    void rotate(float angle, const Vec3& axis);

     Vec4& operator[](i32 i) noexcept {
         return values[i];
     }

    const Vec4& operator[](i32 i) const noexcept {
        return values[i];
     }
};