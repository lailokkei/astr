#include "entities.hpp"
#include "math.h"

Player::Player(Vector2 _position) { transform.position = _position; }

// Vector2 wrapPosition(Vector2 position, Camera arenacamera) {
//     position.x = arenacamera.width % position.x;
//     position.y = arenacamera.height % position.y;
//
//     return position;
// }

void Player::update(double deltaTime) {
    transform.position =
        addVectors(transform.position, scaleVector(velocity, deltaTime));
}

Mesh generateMesh();

Astroid::Astroid(Transform transform, Vector2 velocity)
    : transform{transform}, velocity{velocity}, mesh{generateMesh()} {}

void Astroid::move(double deltaTime) {
    transform.position =
        addVectors(transform.position, scaleVector(velocity, deltaTime));
}

Mesh generateMesh() {
    Mesh astroidMesh = Mesh({Vector2(0, 2), Vector2(1, -1), Vector2(1, -1),
                             Vector2(-1, -1), Vector2(-1, -1), Vector2(0, 2)});

    return astroidMesh;
}
