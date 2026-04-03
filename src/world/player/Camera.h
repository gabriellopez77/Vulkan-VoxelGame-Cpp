#pragma once

#include "math/Vec3.h"
#include "math/Matrix4.h"


class Camera {
public:
    void update(Vec3 position);

    void resize(float width, float height);

    const Matrix4& getProjectionMatrix() const { return m_projection; }
    const Matrix4& getViewMatrix() const { return m_view; }

    Vec3 getDirection() const { return m_direction; }
    Vec3 getPosition() const { return m_position; }

    f32 getRotX() const { return m_rotX; }
    f32 getRotY() const { return m_rotY; }

private:
    void processRotation();

    Matrix4 m_projection{0.f};
    Matrix4 m_view{0.f};

    Vec3 m_position;

    Vec3 m_direction;

    f32 m_rotX = 0.f;
    f32 m_rotY = 0.f;
};
