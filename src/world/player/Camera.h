#pragma once

#include "math/Vec2.h"
#include "math/Vec3.h"
#include "math/Matrix4.h"


class Camera {
public:
    void update(Vec3 position);

    void resize(f32 width, f32 height);

    const Matrix4& getProjectionMatrix() const { return m_projection; }
    const Matrix4& getViewMatrix() const { return m_view; }

    Vec3 getDirection() const { return m_direction; }
    Vec3 getPosition() const { return m_position; }

    f32 getRotX() const { return m_rot.x; }
    f32 getRotY() const { return m_rot.y; }
    Vec2 getRot() const { return m_rot; }

private:
    void processRotation();

    Matrix4 m_projection{0.f};
    Matrix4 m_view{0.f};

    Vec3 m_position;

    Vec3 m_direction;
    Vec2 m_rot;
};
