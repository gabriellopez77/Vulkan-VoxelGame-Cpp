#include "Player.h"

#include "../../Inputs.h"

#include <glm/trigonometric.hpp>

void Player::start(Game* game) {

}

void Player::update(float dt) {
    glm::vec3 dir{};

    float yaw = glm::radians(camera.getRotX());
    glm::vec3 front = { glm::cos(yaw), 0.f, glm::sin(yaw) };

    if (inputs::isKeyDown(inputs::Keys::W)) dir += front;
    if (inputs::isKeyDown(inputs::Keys::A)) dir -= glm::cross(front, {0.f, 1.f, 0.f});
    if (inputs::isKeyDown(inputs::Keys::S)) dir -= front;
    if (inputs::isKeyDown(inputs::Keys::D)) dir += glm::cross(front, {0.f, 1.f, 0.f});
    if (inputs::isKeyDown(inputs::Keys::LeftShift)) dir.y += 1.f;
    if (inputs::isKeyDown(inputs::Keys::Space)) dir.y -= 1.f;

    const float SPEED = 10.f;

    if (glm::length(dir) > 1.0f)
        dir = glm::normalize(dir);

    position += dir * (SPEED * dt);

    camera.update(position);
}
