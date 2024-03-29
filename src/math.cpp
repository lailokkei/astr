#include "math.hpp"
#include <cmath>
#include <format>
#include <iostream>

Vector2::Vector2(double _x, double _y) {
    x = _x;
    y = _y;
}

void Vector2::debug() { std::cout << std::format("x:{}, y:{}\n", x, y); }

Vector2 vector_add(const Vector2& a, const Vector2& b) {
    return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 vector_scale(const Vector2& vec, double magnitude) {
    return Vector2(vec.x * magnitude, vec.y * magnitude);
}

Vector2 vector_rotate(const Vector2& vec, double rad) {
    rad = -rad;
    return Vector2{vec.x * cos(rad) + vec.y * -sin(rad),
                   vec.x * sin(rad) + vec.y * cos(rad)};
}

Mesh::Mesh(std::vector<Vector2> edges) : vertices{edges} {}

void Mesh::debug() {
    std::cout << "mesh:\n";
    for (auto point : vertices) {
        std::cout << std::format("x: {}, y: {}\n", point.x, point.y);
    }
}

bool point_box_collision(Vector2 point, AABB box) {
    auto r = box.position.x + box.dimensions.x / 2;
    auto l = box.position.x - box.dimensions.x / 2;
    auto u = box.position.y + box.dimensions.y / 2;
    auto d = box.position.y - box.dimensions.y / 2;
    if (point.x < l) {
        return false;
    }
    if (point.x > r) {
        return false;
    }
    if (point.y > u) {
        return false;
    }
    if (point.y < d) {
        return false;
    }

    return true;
}

bool collision(AABB h1, AABB h2) {
    auto x1r = h1.position.x + h1.dimensions.x / 2;
    auto x1l = h1.position.x - h1.dimensions.x / 2;
    auto x2r = h2.position.x + h2.dimensions.x / 2;
    auto x2l = h2.position.x - h2.dimensions.x / 2;

    if (x1l > x2r || x1r < x2l) {
        return false;
    }

    auto y1r = h1.position.y + h1.dimensions.y / 2;
    auto y1l = h1.position.y - h1.dimensions.y / 2;
    auto y2r = h2.position.y + h2.dimensions.y / 2;
    auto y2l = h2.position.y - h2.dimensions.y / 2;

    if (y1l > y2r || y1r < y2l) {
        return false;
    }

    return true;
}
