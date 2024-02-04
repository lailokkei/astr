#ifndef ENTITIES_H
#define ENTITIES_H

#include "math.hpp"

struct Camera {
    double width;
    double height;
};

struct Player {
    Vector2 position;
    double angle;
    Vector2 velocity = Vector2(0, 0);

    void update(double);
    Player(Vector2);
};

struct Astroid {
    Vector2 position;
    double angle;
    Vector2 velocity;
    double angularVelocity;
    Mesh mesh;

    int sizeTier;

    void move(double);
    Astroid(Vector2, Vector2, double);
};

#endif
