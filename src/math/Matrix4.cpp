#include "Matrix4.h"

#include "Math.h"
#include "Vec3.h"
#include <glm/gtc/matrix_transform.hpp>


Matrix4 Matrix4::lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
    const Vec3 f(Vec3::normalize(target - eye));
    const Vec3 s(Vec3::normalize(Vec3::cross(f, up)));
    const Vec3 u(Vec3::cross(s, f));

    Matrix4 result(1.f);
    result[0].x =  s.x;
    result[1].x =  s.y;
    result[2].x =  s.z;
    result[0].y =  u.x;
    result[1].y =  u.y;
    result[2].y =  u.z;
    result[0].z = -f.x;
    result[1].z = -f.y;
    result[2].z = -f.z;
    result[3].x = -Vec3::dot(s, eye);
    result[3].y = -Vec3::dot(u, eye);
    result[3].z =  Vec3::dot(f, eye);

    return result;
}

Matrix4 Matrix4::orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
    Matrix4 result(1.f);
    //glm::ortho(0.f, 0.f, 0.f, 0.f);

    result[0].x = 2.f / (right - left);
    result[1].y = 2.f / (top - bottom);
    result[2].z = -1.f;
    result[3].x = -(right + left) / (right - left);
    result[3].y = -(top + bottom) / (top - bottom);

    return result;
}

Matrix4 Matrix4::perspective(f32 fov, f32 aspect, f32 near, f32 far) {
    const f32 tanHalfFov = math::tan(fov / 2.f);

    Matrix4 result(0.f);
    result[0].x = 1.f / (aspect * tanHalfFov);
    result[1].y = 1.f / tanHalfFov;
    result[2].z = - (far + near) / (far - near);
    result[2].w = - 1.f;
    result[3].z = - (2.f * far * near) / (far - near);

    return result;
}

void Matrix4::translate(const Vec3& translation) {
    Matrix4 result(*this);

    result[3] = values[0] * translation.x + values[1] * translation.y + values[2] * translation.z + values[3];

    *this = result;
}

void Matrix4::scale(const Vec3& scale) {
    Matrix4 result(0.f);
    result[0] = values[0] * scale.x;
    result[1] = values[1] * scale.y;
    result[2] = values[2] * scale.z;
    result[3] = values[3];

    *this = result;
}

void Matrix4::rotate(f32 angle, const Vec3& v) {
    const f32 a = math::radians(angle);
    const f32 c = math::cos(a);
    const f32 s = math::sin(a);

    const Vec3 axis = Vec3::normalize(v);
    const Vec3 temp(axis * (1 - c));

    Matrix4 rotate(0.f);
    rotate[0].x = c + temp.x * axis.x;
    rotate[0].y = temp.x * axis.y + s * axis.z;
    rotate[0].z = temp.x * axis.z - s * axis.y;

    rotate[1].x = temp.y * axis.x - s * axis.z;
    rotate[1].y = c + temp.y * axis.y;
    rotate[1].z = temp.y * axis.z + s * axis.x;

    rotate[2].x = temp.z * axis.x + s * axis.y;
    rotate[2].y = temp.z * axis.y - s * axis.x;
    rotate[2].z = c + temp.z * axis.z;

    Matrix4 result(0.f);
    result[0] = values[0] * rotate[0].x + values[1] * rotate[0].y + values[2] * rotate[0].z;
    result[1] = values[0] * rotate[1].x + values[1] * rotate[1].y + values[2] * rotate[1].z;
    result[2] = values[0] * rotate[2].x + values[1] * rotate[2].y + values[2] * rotate[2].z;
    result[3] = values[3];

    *this = result;
}