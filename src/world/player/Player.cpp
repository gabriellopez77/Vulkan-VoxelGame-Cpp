#include "Player.h"

#include "Inputs.h"


#include "math/math.h"

void Player::start(Game* game) {

}

void Player::update(f32 dt) {
    Vec3 dir;

    float yaw = math::radians(camera.getRotX());
    Vec3 front = { math::cos(yaw), 0.f, math::sin(yaw) };

    if (inputs::isKeyDown(inputs::Keys::W)) dir += front;
    if (inputs::isKeyDown(inputs::Keys::A)) dir -= Vec3::cross(front, { 0.f, 1.f, 0.f });
    if (inputs::isKeyDown(inputs::Keys::S)) dir -= front;
    if (inputs::isKeyDown(inputs::Keys::D)) dir += Vec3::cross(front, { 0.f, 1.f, 0.f });
    if (inputs::isKeyDown(inputs::Keys::LeftShift)) dir.y++;
    if (inputs::isKeyDown(inputs::Keys::Space)) dir.y--;

    const float SPEED = 10.f;

    if (dir.length() > 1.0f)
        dir.normalize();

    position += dir * (SPEED * dt);

    camera.update(position);
}
