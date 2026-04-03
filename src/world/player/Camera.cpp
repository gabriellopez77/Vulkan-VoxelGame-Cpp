#include "Camera.h"

#include "Inputs.h"
#include <glm/trigonometric.hpp>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "math/Math.h"


void Camera::update(Vec3 position) {
    m_position = {position.x, position.y, position.z};

    processRotation();

    m_view = Matrix4::lookAt(m_position, m_position + m_direction, { 0.f, 1.f, 0.f });
}

void Camera::resize(float width, float height) {
    m_projection = Matrix4::perspective(glm::radians(70.f), width / height, 0.1f, 1000.f);
}

void Camera::processRotation() {
    const float sensitivity = 0.7f;

    auto delta = inputs::getMouseDelta() * sensitivity;

    m_rotX += delta.x;
    m_rotY += delta.y;


    m_rotY = math::clamp(m_rotY, -89.f, 89.f);

    const Vec3 direction = {
        glm::cos(math::radians(m_rotX)) * glm::cos(math::radians(m_rotY)),
        glm::sin(math::radians(m_rotY)),
        glm::sin(math::radians(m_rotX)) * glm::cos(math::radians(m_rotY))
    };

    m_direction = direction.normalized();
}