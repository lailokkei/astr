#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <cmath>
#include <format>
#include <iostream>
#include <numbers>
#include <vector>

#include "entities.hpp"
#include "math.hpp"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

const double angularVelocity = 180 * std::numbers::pi / 180;
const int acceleration = 20;

struct ScreenPoint {
    int x;
    int y;
};

ScreenPoint worldToScreen(Vector2 point, Camera camera) {
    int x = (point.x / camera.width) * SCREEN_WIDTH;
    int y = (camera.height - point.y) / camera.height * SCREEN_HEIGHT;
    return ScreenPoint{x, y};
}

void renderMesh(SDL_Renderer* renderer, Mesh mesh, Camera camera) {
    std::vector<Vector2>& vertices = mesh.vertices;

    for (int i = 0; i < mesh.vertices.size() - 1; i++) {
        auto p1 = worldToScreen(vertices[i], camera);
        auto p2 = worldToScreen(vertices[i + 1], camera);
        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }
}

// void renderBox(SDL_Renderer* renderer, Hitbox box, Camera camera) {
//     std::array<Vector2, 4> vertices = {Vector2(0, 0)};
//     if
// }

Mesh transformMesh(Mesh mesh, Transform transform) {
    for (auto& point : mesh.vertices) {
        auto angle = -transform.angle;
        point = vectorAdd(vectorRotate(point, angle), transform.position);
    }

    return mesh;
}

struct Hitbox {
    Vector2 position;
    Vector2 dimensions;
};

// bool collision(Hitbox h1, Hitbox h2) {
//     if (h1.position.x + h1.dimensions.x / 2 > h2.position - h2.dimensions.x)
//         return false;
// }

Vector2 wrapPoint(Vector2 point, Camera camera) {
    if (point.x < 0) {
        point.x += camera.width;
    }
    if (point.x > camera.width) {
        point.x -= camera.width;
    }
    if (point.y < 0) {
        point.y += camera.height;
    }
    if (point.y > camera.height) {
        point.y -= camera.height;
    }
    return point;
}

struct GameState {
    const Uint8* keyStates = SDL_GetKeyboardState(NULL);

    Mesh playerMesh =
        Mesh({Vector2(0, 2), Vector2(1, -1), Vector2(-1, -1), Vector2(0, 2)});

    Player player = Player(Vector2(50, 36));
    std::vector<Astroid> astroids{};
    Camera camera = Camera{100, 75};

    GameState();
    void poll();
    void update(double);
    void render(SDL_Renderer*);
};

GameState::GameState() {
    astroids.push_back(
        Astroid(Transform{Vector2(20, 36), 0}, Vector2{2, 1}, 0.5));
    astroids.push_back(
        Astroid(Transform{Vector2(0, 2), 0}, Vector2{2, 1}, -0.1));
    astroids.push_back(
        Astroid(Transform{Vector2(90, 4), 0}, Vector2{-4, 1}, 0.2));
}

void GameState::update(double deltaTime) {
    if (keyStates[SDL_SCANCODE_W]) {
        auto angle{player.transform.angle};
        auto direction{Vector2(sin(angle), cos(angle))};
        player.velocity = vectorAdd(
            player.velocity, vectorScale(direction, acceleration * deltaTime));
    }

    if (keyStates[SDL_SCANCODE_A]) {
        player.transform.angle -= angularVelocity * deltaTime;
    }

    if (keyStates[SDL_SCANCODE_D]) {
        player.transform.angle += angularVelocity * deltaTime;
    }

    player.update(deltaTime);

    player.transform.position = wrapPoint(player.transform.position, camera);

    for (auto& astroid : astroids) {
        astroid.transform.position =
            vectorAdd(astroid.transform.position,
                      vectorScale(astroid.velocity, deltaTime));
        astroid.transform.position =
            wrapPoint(astroid.transform.position, camera);
        astroid.transform.angle += astroid.angularVelocity * deltaTime;
    }
}

void GameState::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

    renderMesh(renderer, transformMesh(playerMesh, player.transform), camera);

    for (auto astroid : astroids) {
        renderMesh(renderer, transformMesh(astroid.mesh, astroid.transform),
                   camera);
    }

    SDL_RenderPresent(renderer);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1) {
        std::cout << std::format("{}\n", SDL_GetError());
        return 1;
    };

    SDL_Window* window =
        SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         1024, 768, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event e;
    bool quit = false;
    auto lastFrame = SDL_GetTicks();

    GameState gameState = GameState();

    while (!quit) {
        auto curentFrame = SDL_GetTicks();
        double deltaTime = double(curentFrame - lastFrame) / 1000;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        gameState.update(deltaTime);
        gameState.render(renderer);

        lastFrame = curentFrame;
    }

    SDL_Quit();

    return 0;
}
