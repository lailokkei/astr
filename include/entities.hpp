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
    Vector2 hitbox = Vector2{2, 3};
    Mesh mesh =
        Mesh({Vector2(0, 2), Vector2(1, -1), Vector2(-1, -1), Vector2(0, 2)});

    void update(double);
    Player(Vector2);
};

struct Astroid {
    Vector2 position;
    double angle;
    Vector2 velocity;
    double angularVelocity;
    Mesh mesh;
    Vector2 hitbox = {14, 14};

    int size = 0;

    void move(double);
    Astroid(Vector2, Vector2, double, int);
};

struct Bullet {
    Vector2 position;
    Vector2 velocity;
    Vector2 hitbox = {1, 1};
};

#endif
