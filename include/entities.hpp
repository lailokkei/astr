#ifndef ENTITIES_H
#define ENTITIES_H

#include "math.hpp"

struct Camera {
    double width;
    double height;
};

struct Player {
    Transform transform = Transform(Vector2(0, 0), 0);
    Vector2 velocity = Vector2(0, 0);

    void update(double);
    Player(Vector2);
};

struct Astroid {
    Transform transform;
    Vector2 velocity;
    Mesh mesh;

    int sizeTier;

    void move(double);
    Astroid(Transform, Vector2);
};

#endif
