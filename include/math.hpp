#ifndef MATH_H
#define MATH_H

#include <vector>

struct Vector2 {
    double x, y;

    Vector2(double, double);
    void debug();
};

struct Mesh {
    std::vector<Vector2> vertices;

    Mesh(std::vector<Vector2>);
    void debug();
};

struct Transform {
    Vector2 position;
    double angle;
};

Vector2 vectorAdd(const Vector2&, const Vector2&);

Vector2 vectorScale(const Vector2&, double);

Vector2 vectorRotate(const Vector2&, double);

#endif
