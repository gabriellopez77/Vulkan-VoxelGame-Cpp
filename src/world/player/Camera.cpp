#include "Camera.h"

#include "Inputs.h"

#include "math/Math.h"


void Camera::update(Vec3 position) {
    m_position = {position.x, position.y, position.z};

    processRotation();

    m_view = Matrix4::lookAt(m_position, m_position + m_direction, { 0.f, 1.f, 0.f });
}

void Camera::resize(f32 width, f32 height) {
    m_projection = Matrix4::perspective(math::radians(70.f), width / height, 0.1f, 1000.f);
}

void Camera::processRotation() {
    const f32 sensitivity = 0.7f;

    auto delta = inputs::getMouseDelta() * sensitivity;

    m_rot.x += delta.x;
    m_rot.y += delta.y;


    m_rot.y = math::clamp(m_rot.y, -89.f, 89.f);

    const Vec3 direction = {
        math::cos(math::radians(m_rot.x)) * math::cos(math::radians(m_rot.y)),
        math::sin(math::radians(m_rot.y)),
        math::sin(math::radians(m_rot.x)) * math::cos(math::radians(m_rot.y))
    };

    m_direction = direction.normalized();
}