#ifndef ENTITIES_H
#define ENTITIES_H

#include "math.hpp"

struct Camera {
    double width;
    double height;
};

struct Player {
    Transform transform;
    Vector2 velocity = Vector2(0, 0);

    void update(double);
    Player(Vector2);
};

struct Astroid {
    Transform transform;
    Vector2 velocity;
    double angularVelocity;
    double angle = 0;
    Mesh mesh;

    int sizeTier;

    void move(double);
    Astroid(Transform, Vector2, double);
};

#endif
