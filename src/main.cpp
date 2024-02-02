#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
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

const double angularVelocity = 160 * std::numbers::pi / 180;
const int acceleration = 100;

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
    std::vector<Vector2>& edges = mesh.edges;

    for (int i = 0; i < mesh.edges.size(); i += 2) {
        auto p1 = worldToScreen(edges[i], camera);
        auto p2 = worldToScreen(edges[i + 1], camera);
        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }
}

Mesh transformMesh(Mesh mesh, Transform transform) {
    for (auto& point : mesh.edges) {
        auto angle = -transform.angle;

        Vector2 newPoint = Vector2(0, 0);

        newPoint.x = point.x * cos(angle) + point.y * -sin(angle);
        newPoint.y = point.x * sin(angle) + point.y * cos(angle);

        newPoint.x += transform.position.x;
        newPoint.y += transform.position.y;

        point = newPoint;
    }

    return mesh;
}

struct GameState {};

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
    // point.x = std::fmod(point.x, camera.width);
    // point.y = std::fmod(point.y, camera.height);
    return point;
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1) {
        std::cout << std::format("{}\n", SDL_GetError());
        return 1;
    };

    SDL_Window* window = SDL_CreateWindow(
        "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768,
        SDL_WINDOW_SHOWN); //| SDL_WINDOW_RESIZABLE);

    // window

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event e;
    bool quit = false;

    Mesh playerMesh = Mesh({Vector2(0, 2), Vector2(1, -1), Vector2(1, -1),
                            Vector2(-1, -1), Vector2(-1, -1), Vector2(0, 2)});

    Player player = Player(Vector2(50, 36));
    for (auto& point : playerMesh.edges) {
        point.x *= 3;
        point.y *= 3;
    }

    std::vector<Astroid> astroids{};
    astroids.push_back(Astroid{Transform{Vector2(300, 500), 0}, Vector2{0, 0}});

    auto camera = Camera{100, 75};

    auto lastFrame = SDL_GetTicks();

    bool held = false;

    auto keyStates = SDL_GetKeyboardState(NULL);

    while (!quit) {
        auto curentFrame = SDL_GetTicks();
        double deltaTime = double(curentFrame - lastFrame) / 1000;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        if (keyStates[SDL_SCANCODE_W]) {
            auto angle{player.transform.angle};
            auto direction{Vector2(sin(angle), cos(angle))};
            player.velocity =
                addVectors(player.velocity,
                           scaleVector(direction, acceleration * deltaTime));
        }

        if (keyStates[SDL_SCANCODE_A]) {
            player.transform.angle -= angularVelocity * deltaTime;
        }

        if (keyStates[SDL_SCANCODE_D]) {
            player.transform.angle += angularVelocity * deltaTime;
        }

        player.update(deltaTime);

        player.transform.position =
            wrapPoint(player.transform.position, camera);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

        renderMesh(renderer, transformMesh(playerMesh, player.transform),
                   camera);

        for (auto astroid : astroids) {
            renderMesh(renderer, transformMesh(astroid.mesh, astroid.transform),
                       camera);
        }

        SDL_RenderPresent(renderer);

        lastFrame = curentFrame;
    }

    SDL_Quit();

    return 0;
}
