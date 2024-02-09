#ifndef MATH_H
#define MATH_H

#include <vector>

struct Vector2 {
    double x, y;

    Vector2(double, double);
    void debug();
};

struct AABB {
    Vector2 position;
    Vector2 dimensions;
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

Vector2 vector_add(const Vector2&, const Vector2&);

Vector2 vector_scale(const Vector2&, double);

Vector2 vector_rotate(const Vector2&, double);

bool point_box_collision(Vector2, AABB);
bool collision(AABB, AABB);

#endif
