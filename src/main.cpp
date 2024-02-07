#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <format>
#include <iostream>
#include <numbers>
#include <random>
#include <vector>

#include "entities.hpp"
#include "math.hpp"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

const double angularVelocity = 220 * std::numbers::pi / 180;
const int acceleration = 20;
const double bulletVelocity = 100;

void remove_bullet(std::vector<Bullet>& bullets, int i);

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

Mesh boxMesh(Hitbox box) {
    auto r = box.position.x + box.dimensions.x / 2;
    auto l = box.position.x - box.dimensions.x / 2;
    auto u = box.position.y + box.dimensions.y / 2;
    auto d = box.position.y - box.dimensions.y / 2;
    return Mesh({{l, u}, {r, u}, {r, d}, {l, d}, {l, u}});
}
void renderBox(SDL_Renderer* renderer, Hitbox box, Camera cam) {}

Mesh transformMesh(Mesh mesh, Vector2 position, double angle) {
    for (auto& point : mesh.vertices) {
        point = vectorAdd(vectorRotate(point, angle), position);
    }

    return mesh;
}

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

bool out_of_bounds(Vector2 point, Camera cam) {
    if (point.x < 0 || point.x > cam.width) {
        return true;
    }
    if (point.y < 0 || point.y > cam.height) {
        return true;
    }

    return false;
}

struct GameState {
    Uint8* lastKeyStates;
    const Uint8* keyStates = SDL_GetKeyboardState(NULL);

    Player player = Player(Vector2(50, 36));
    std::vector<Astroid> astroids{};
    std::vector<Bullet> bullets{};
    Camera camera = Camera{100, 75};

    bool idk{};
    bool ded{};

    GameState();
    void poll();
    void update(double);
    void render(SDL_Renderer*);
};

GameState::GameState() {
    astroids.push_back(Astroid(Vector2(20, 36), Vector2{2, 1}, 0.5, 6));
    astroids.push_back(Astroid(Vector2(0, 2), Vector2{2, 1}, -0.1, 4));
    astroids.push_back(Astroid(Vector2(90, 4), Vector2{-4, 1}, 0.2, 4));
}

void splitAstroid(std::vector<Astroid>& astroids, int i) {
    auto& parent = astroids[i];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> angleRange(0, 2 * std::numbers::pi);

    Vector2 v1 =
        vectorAdd(vectorRotate({0, 3}, angleRange(gen)), parent.velocity);
    Vector2 v2 =
        vectorAdd(vectorRotate({0, 3}, angleRange(gen)), parent.velocity);
    if (parent.size > 1) {
        astroids.push_back(Astroid(parent.position, v1, -1, parent.size - 1));
        astroids.push_back(Astroid(parent.position, v2, 1, parent.size - 1));
    }

    astroids.erase(astroids.begin() + i);
}

void GameState::update(double deltaTime) {
    if (keyStates[SDL_SCANCODE_W]) {
        auto angle{player.angle};
        auto direction{Vector2(sin(angle), cos(angle))};
        player.velocity = vectorAdd(
            player.velocity, vectorScale(direction, acceleration * deltaTime));
    }

    if (keyStates[SDL_SCANCODE_A]) {
        player.angle -= angularVelocity * deltaTime;
    }

    if (keyStates[SDL_SCANCODE_D]) {
        player.angle += angularVelocity * deltaTime;
    }

    player.update(deltaTime);
    player.position = wrapPoint(player.position, camera);

    for (int i = bullets.size() - 1; i >= 0; i--) {
        auto& bullet = bullets[i];
        bullet.position =
            vectorAdd(bullet.position, vectorScale(bullet.velocity, deltaTime));
        if (out_of_bounds(bullet.position, camera)) {
            remove_bullet(bullets, i);
        }
    }

    for (int i = bullets.size() - 1; i >= 0; i--) {
        for (int j = astroids.size() - 1; j >= 0; j--) {
            auto& astroid = astroids[j];
            auto box = Hitbox{astroid.position, astroid.hitbox};
            if (point_box_collision(bullets[i].position, box)) {
                splitAstroid(astroids, j);
                remove_bullet(bullets, i);
                break;
            }
        }
    }

    for (int i = astroids.size() - 1; i >= 0; i--) {
        auto& astroid = astroids[i];
        astroid.position = vectorAdd(astroid.position,
                                     vectorScale(astroid.velocity, deltaTime));
        astroid.position = wrapPoint(astroid.position, camera);
        astroid.angle += astroid.angularVelocity * deltaTime;

        if (collision(Hitbox{player.position, player.hitbox},
                      Hitbox{astroid.position, astroid.hitbox})) {
            ded = true;
        }
    }

    if (idk) {
        bullets.push_back(Bullet{
            player.position, vectorRotate({0, bulletVelocity}, player.angle)});
    }
}

void remove_bullet(std::vector<Bullet>& bullets, int i) {
    bullets.erase(bullets.begin() + i);
}

void GameState::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

    renderMesh(renderer,
               transformMesh(player.mesh, player.position, player.angle),
               camera);

    for (auto astroid : astroids) {
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        renderMesh(renderer,
                   transformMesh(astroid.mesh, astroid.position, astroid.angle),
                   camera);

        // SDL_SetRenderDrawColor(renderer, 0xff, 0x0, 0x0, 0xff);
        // renderMesh(renderer,
        //            boxMesh(Hitbox{{astroid.position}, astroid.hitbox}),
        //            camera);
    }

    for (auto bullet : bullets) {
        SDL_SetRenderDrawColor(renderer, 0xff, 0x0, 0x0, 0xff);
        renderMesh(renderer, boxMesh(Hitbox{{bullet.position}, bullet.hitbox}),
                   camera);
    }
    //
    // SDL_SetRenderDrawColor(renderer, 0xff, 0x0, 0x0, 0xff);
    // renderMesh(renderer, boxMesh(Hitbox{{player.position}, player.hitbox}),
    //            camera);

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

    std::array<Uint32, 10> frameBuffer = {};
    std::fill(frameBuffer.begin(), frameBuffer.end(), 0);

    GameState gameState = GameState();

    std::cout << "\033[2J";

    while (!quit && !gameState.ded) {
        auto currentFrame = SDL_GetTicks();
        double deltaTime = double(currentFrame - lastFrame) / 1000.0;

        for (int i = frameBuffer.size() - 1; i > 0; i--) {
            frameBuffer[i] = frameBuffer[i - 1];
        }
        frameBuffer[0] = currentFrame;

        double fps =
            frameBuffer.size() /
            ((frameBuffer[0] - frameBuffer[frameBuffer.size() - 1]) / 1000.0);

        // for (auto i : frameBuffer) {
        //     std::cout << i << " ";
        // }
        // std::cout << "\n";

        std::cout << "\033[H";
        std::cout << std::format("{}\n", fps);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_SPACE) {
                    gameState.idk = true;
                }
            }
        }

        gameState.update(deltaTime);
        gameState.render(renderer);

        lastFrame = currentFrame;
        gameState.idk = false;
    }

    SDL_Quit();

    return 0;
}
