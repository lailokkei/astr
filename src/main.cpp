#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <cmath>
#include <format>
#include <iostream>
#include <numbers>
#include <vector>

#include "main.h"
#include "math.h"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

const double angularVelocity = 90 * std::numbers::pi / 180;

Player::Player(Vector2 _position) { transform.position = _position; }

void Player::update(double deltaTime) {
    transform.position.x += velocity.x * deltaTime;
    transform.position.y += velocity.y * deltaTime;
}

void renderMesh(SDL_Renderer* renderer, Mesh mesh) {
    std::vector<Vector2>& edges = mesh.edges;
    for (int i = 0; i < mesh.edges.size(); i += 2) {
        SDL_RenderDrawLine(renderer, edges[i].x, SCREEN_HEIGHT - edges[i].y,
                           edges[i + 1].x, SCREEN_HEIGHT - edges[i + 1].y);
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

    Mesh playerMesh = Mesh({Vector2(0, 2), Vector2(1, -1), Vector2(1, -1),
                            Vector2(-1, -1), Vector2(-1, -1), Vector2(0, 2)});

    Player player = Player(Vector2(500, 300));
    for (auto& point : playerMesh.edges) {
        point.x *= 30;
        point.y *= 30;
    }

    auto lastFrame = SDL_GetTicks();

    bool held = false;

    while (!quit) {
        auto curentFrame = SDL_GetTicks();
        double deltaTime = double(curentFrame - lastFrame) / 1000;

        std::cout << std::format("{}\n", deltaTime);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_w:
                    held = true;
                }
            }

            if (e.type == SDL_KEYUP) {
                // switch (e.key.keysym.sym) {
                // case SDLK_w:
                held = false;
                // }
            }
        }

        if (held == true) {
            player.transform.angle += angularVelocity * deltaTime;
        }

        // player.update(deltaTime);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

        renderMesh(renderer, transformMesh(playerMesh, player.transform));

        SDL_RenderPresent(renderer);

        lastFrame = curentFrame;
    }

    SDL_Quit();

    return 0;
}
