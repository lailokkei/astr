#include "math.hpp"
#include <format>
#include <iostream>

Vector2::Vector2(double _x, double _y) {
    x = _x;
    y = _y;
}

void Vector2::debug() { std::cout << std::format("x:{}, y:{}\n", x, y); }

Vector2 addVectors(const Vector2& a, const Vector2& b) {
    return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 scaleVector(const Vector2& vec, double magnitude) {
    return Vector2(vec.x * magnitude, vec.y * magnitude);
}

Mesh::Mesh(std::vector<Vector2> _edges) { edges = _edges; }

void Mesh::debug() {
    std::cout << "mesh:\n";
    for (auto point : edges) {
        std::cout << std::format("x: {}, y: {}\n", point.x, point.y);
    }
}
