#include "entities.hpp"
#include "math.h"
#include "math.hpp"
#include <algorithm>
#include <numbers>
#include <random>

Player::Player(Vector2 position) : transform{Transform{position, 0}} {}

// Vector2 wrapPosition(Vector2 position, Camera arenacamera) {
//     position.x = arenacamera.width % position.x;
//     position.y = arenacamera.height % position.y;
//
//     return position;
// }

void Player::update(double deltaTime) {
    transform.position =
        vectorAdd(transform.position, vectorScale(velocity, deltaTime));
}

Mesh generateMesh();

Astroid::Astroid(Transform transform, Vector2 velocity, double angularVelocity)
    : transform{transform}, velocity{velocity},
      angularVelocity{angularVelocity}, mesh{generateMesh()} {}

void Astroid::move(double deltaTime) {
    transform.position =
        vectorAdd(transform.position, vectorScale(velocity, deltaTime));
}

Mesh generateMesh() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 2 * std::numbers::pi);
    std::uniform_real_distribution<> radiusRange(5, 8);

    // vectorRotate(const Vector2&, double)
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
