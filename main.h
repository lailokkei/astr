#ifndef MAIN_H
#define MAIN_H

#include "math.h"

struct Player {
    Transform transform = Transform(Vector2(0, 0), 0);
    Vector2 velocity = Vector2(0, 0);

    void update();
    Player(Vector2);
};

#endif
