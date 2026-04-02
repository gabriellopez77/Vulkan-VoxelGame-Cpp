#include "Camera.h"

#include "../../Inputs.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"


void Camera::update(glm::vec3 position) {
    m_position = position;

    processRotation();

    m_view = glm::lookAt(m_position, m_position + m_direction, { 0.f, 1.f, 0.f });
}

void Camera::resize(float width, float height) {
    m_projection = glm::perspective(glm::radians(70.f), width / height, 0.1f, 1000.f);
}

void Camera::processRotation() {
    const float sensitivity = 0.7f;

    auto delta = inputs::getMouseDelta() * sensitivity;

    m_rotX += delta.x;
    m_rotY += delta.y;

    m_rotY = glm::clamp(m_rotY, -89.f, 89.f);

    glm::vec3 direction = {
        glm::cos(glm::radians(m_rotX)) * glm::cos(glm::radians(m_rotY)),
        glm::sin(glm::radians(m_rotY)),
        glm::sin(glm::radians(m_rotX)) * glm::cos(glm::radians(m_rotY))
    };

    m_direction = glm::normalize(direction);
}
