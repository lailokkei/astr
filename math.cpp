#include "math.h"
#include <format>
#include <iostream>

Vector2::Vector2(int _x, int _y) {
    x = _x;
    y = _y;
}

Mesh::Mesh(std::vector<Vector2> _edges) { edges = _edges; }

void Mesh::debug() {
    std::cout << "mesh:\n";
    for (auto point : edges) {
        std::cout << std::format("x: {}, y: {}\n", point.x, point.y);
    }
}
