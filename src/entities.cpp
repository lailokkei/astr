#include "entities.hpp"
#include "math.h"
#include "math.hpp"
#include <numbers>
#include <random>

Player::Player(Vector2 position) : position{position} {}

void Player::update(double deltaTime) {
    position = vectorAdd(position, vectorScale(velocity, deltaTime));
}

Mesh generateAstroidMesh(double radius);

Astroid::Astroid(Vector2 position, Vector2 velocity, double angularVelocity,
                 int size)
    : position{position}, velocity{velocity} {
    auto length = double(size) * 2 * 2;
    hitbox = {length, length};
    this->mesh = generateAstroidMesh(size * 2);
    this->angularVelocity = angularVelocity;
    this->size = size;
}

void Astroid::move(double deltaTime) {
    position = vectorAdd(position, vectorScale(velocity, deltaTime));
}

Mesh generateAstroidMesh(double radius) {
    double noise = radius * 0.2;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 2 * std::numbers::pi);
    std::uniform_real_distribution<> radiusRange(radius - noise,
                                                 radius + noise);

    std::vector<double> directions;

    double dir = 0;
    while (dir < 2 * std::numbers::pi) {
        directions.push_back(dir);
        dir += 1.05;
    }

    Mesh astroidMesh = Mesh({});

    for (auto dir : directions) {
        auto vertex =
            vectorScale(vectorRotate(Vector2{0, 1}, dir), radiusRange(gen));
        astroidMesh.vertices.push_back(vertex);
    }
    astroidMesh.vertices.push_back(astroidMesh.vertices[0]);

    return astroidMesh;
}
