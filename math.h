#ifndef MATH_H
#define MATH_H

#include <vector>

struct Vector2 {
    int x, y;

    Vector2(int, int);
};

struct Mesh {
    std::vector<Vector2> edges;

    Mesh(std::vector<Vector2>);
    void debug();
};

struct Transform {
    Vector2 position;
    double angle;
};

#endif
