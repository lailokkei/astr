#include "math.hpp"
#include <cmath>
#include <format>
#include <iostream>

Vector2::Vector2(double _x, double _y) {
    x = _x;
    y = _y;
}

void Vector2::debug() { std::cout << std::format("x:{}, y:{}\n", x, y); }

Vector2 vectorAdd(const Vector2& a, const Vector2& b) {
    return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 vectorScale(const Vector2& vec, double magnitude) {
    return Vector2(vec.x * magnitude, vec.y * magnitude);
}

Vector2 vectorRotate(const Vector2& vec, double rad) {
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
