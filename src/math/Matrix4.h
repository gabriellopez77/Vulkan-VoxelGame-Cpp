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

	Matrix4(const Vec4& v1, const Vec4& v2, const Vec4& v3, const Vec4& v4)
		: values{ v1, v2, v3, v4 } {}

    static Matrix4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up);
    static Matrix4 orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
    static Matrix4 perspective(f32 fov, f32 aspect, f32 near, f32 far);

    Vec4 values[4];

    void translate(const Vec3& translation);
    void scale(const Vec3& scale);
    void rotate(f32 angle, const Vec3& axis);

    Matrix4 operator* (const Matrix4& other) const {
		const Vec4& SrcA0 = this->values[0];
		const Vec4& SrcA1 = this->values[1];
		const Vec4& SrcA2 = this->values[2];
		const Vec4& SrcA3 = this->values[3];

		const Vec4& SrcB0 = other[0];
		const Vec4& SrcB1 = other[1];
		const Vec4& SrcB2 = other[2];
		const Vec4& SrcB3 = other[3];

		return {
			SrcA3* SrcB0.w + SrcA2 * SrcB0.z + SrcA1 * SrcB0.y + SrcA0 * SrcB0.x,
			SrcA3* SrcB1.w + SrcA2 * SrcB1.z + SrcA1 * SrcB1.y + SrcA0 * SrcB1.x,
			SrcA3* SrcB2.w + SrcA2 * SrcB2.z + SrcA1 * SrcB2.y + SrcA0 * SrcB2.x,
			SrcA3* SrcB3.w + SrcA2 * SrcB3.z + SrcA1 * SrcB3.y + SrcA0 * SrcB3.x
		};
    }

    Vec4& operator[](i32 i) noexcept { return values[i]; }
    const Vec4& operator[](i32 i) const noexcept { return values[i]; }
};