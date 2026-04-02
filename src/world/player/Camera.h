#pragma once

#include "../../renderer/Defs.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>


class Camera {
public:
    void update(glm::vec3 position);

    void resize(float width, float height);

    const glm::mat4& getProjectionMatrix() const { return m_projection; }
    const glm::mat4& getViewMatrix() const { return m_view; }

    glm::vec3 getDirection() const { return m_direction; }
    glm::vec3 getPosition() const { return m_position; }

    f32 getRotX() const { return m_rotX; }
    f32 getRotY() const { return m_rotY; }

private:
    void processRotation();

    glm::mat4 m_projection{};
    glm::mat4 m_view{};

    glm::vec3 m_position{};

    glm::vec3 m_direction{};

    f32 m_rotX = 0.f;
    f32 m_rotY = 0.f;
};
