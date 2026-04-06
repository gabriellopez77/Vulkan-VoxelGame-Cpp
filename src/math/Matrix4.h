#pragma once

#include "Vec4.h"


// fwd
class Vec3;

class Matrix4 {
public:
    Matrix4(f32 value) {
        values[0].x = value;
        values[1].y = value;
        values[2].z = value;
        values[3].w = value;
    }

    static Matrix4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up);
    static Matrix4 orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
    static Matrix4 perspective(f32 fov, f32 aspect, f32 near, f32 far);

    Vec4 values[4];

    void translate(const Vec3& translation);
    void scale(const Vec3& scale);
    void rotate(f32 angle, const Vec3& axis);

     Vec4& operator[](i32 i) noexcept { return values[i]; }
    const Vec4& operator[](i32 i) const noexcept { return values[i]; }
};