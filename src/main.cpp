#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
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

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;

const double angularVelocity = 220 * std::numbers::pi / 180;
const int acceleration = 30;
const double bulletVelocity = 100;
const double particleVelocity = 50;
const float particleSize = 1;

void remove_bullet(std::vector<Bullet>& bullets, int i);

struct ScreenPoint {
    int x;
    int y;
};

ScreenPoint world_to_screen(Vector2 point, Camera camera) {
    int x = (point.x / camera.width) * SCREEN_WIDTH;
    int y = (camera.height - point.y) / camera.height * SCREEN_HEIGHT;
    return ScreenPoint{x, y};
}

void render_mesh(SDL_Renderer* renderer, Mesh mesh, Camera camera) {
    std::vector<Vector2>& vertices = mesh.vertices;

    for (int i = 0; i < mesh.vertices.size() - 1; i++) {
        auto p1 = world_to_screen(vertices[i], camera);
        auto p2 = world_to_screen(vertices[i + 1], camera);
        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }
}

void render_AABB(SDL_Renderer* renderer, AABB box, Camera cam) {}

Mesh box_mesh(AABB box) {
    auto r = box.position.x + box.dimensions.x / 2;
    auto l = box.position.x - box.dimensions.x / 2;
    auto u = box.position.y + box.dimensions.y / 2;
    auto d = box.position.y - box.dimensions.y / 2;
    return Mesh({{l, u}, {r, u}, {r, d}, {l, d}, {l, u}});
}
void render_box(SDL_Renderer* renderer, AABB box, Camera cam) {}

Mesh transform_mesh(Mesh mesh, Vector2 position, double angle) {
    for (auto& point : mesh.vertices) {
        point = vector_add(vector_rotate(point, angle), position);
    }

    return mesh;
}

Vector2 wrap_point(Vector2 point, Camera camera) {
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
    std::vector<Particle> particles{};
    Camera camera = Camera{100, 75};

    bool spaceDown{};
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

void split_astroid(std::vector<Astroid>& astroids, int i) {
    auto& parent = astroids[i];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> angleRange(0, 2 * std::numbers::pi);

    Vector2 v1 =
        vector_add(vector_rotate({0, 3}, angleRange(gen)), parent.velocity);
    Vector2 v2 =
        vector_add(vector_rotate({0, 3}, angleRange(gen)), parent.velocity);
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
        player.velocity = vector_add(
            player.velocity, vector_scale(direction, acceleration * deltaTime));

        int particleCount = particles.size();
        if (particleCount == 0 ||
            particles[particleCount - 1].timeElapsed > 0.02) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> angleRange(0, 0.2);
            Vector2 velocity = vector_rotate(Vector2(0, -particleVelocity),
                                             player.angle + angleRange(gen));
            auto newParticle = Particle{
                .position = vector_add(
                    player.position,
                    vector_rotate(Vector2{0, -particleSize}, player.angle)),
                .velocity = vector_add(velocity, player.velocity)};
            particles.push_back(newParticle);
        }
    }

    if (keyStates[SDL_SCANCODE_A]) {
        player.angle -= angularVelocity * deltaTime;
    }

    if (keyStates[SDL_SCANCODE_D]) {
        player.angle += angularVelocity * deltaTime;
    }

    player.update(deltaTime);
    player.position = wrap_point(player.position, camera);

    for (int i = particles.size() - 1; i >= 0; i--) {
        auto& particle = particles[i];
        particle.position = vector_add(
            particle.position, vector_scale(particle.velocity, deltaTime));
        particle.timeElapsed += deltaTime;
        if (particle.timeElapsed > particle.lifetime) {
            particles.erase(particles.begin() + i);
        }
    }

    for (int i = bullets.size() - 1; i >= 0; i--) {
        auto& bullet = bullets[i];
        bullet.position = vector_add(bullet.position,
                                     vector_scale(bullet.velocity, deltaTime));
        if (out_of_bounds(bullet.position, camera)) {
            remove_bullet(bullets, i);
        }
    }

    for (int i = bullets.size() - 1; i >= 0; i--) {
        for (int j = astroids.size() - 1; j >= 0; j--) {
            auto& astroid = astroids[j];
            auto box = AABB{astroid.position, astroid.hitbox};
            if (point_box_collision(bullets[i].position, box)) {
                split_astroid(astroids, j);
                remove_bullet(bullets, i);
                break;
            }
        }
    }

    for (int i = astroids.size() - 1; i >= 0; i--) {
        auto& astroid = astroids[i];
        astroid.position = vector_add(
            astroid.position, vector_scale(astroid.velocity, deltaTime));
        astroid.position = wrap_point(astroid.position, camera);
        astroid.angle += astroid.angularVelocity * deltaTime;

        if (collision(AABB{player.position, player.hitbox},
                      AABB{astroid.position, astroid.hitbox})) {
            ded = true;
        }
    }

    if (spaceDown) {
        bullets.push_back(Bullet{
            player.position, vector_rotate({0, bulletVelocity}, player.angle)});
    }
}

void remove_bullet(std::vector<Bullet>& bullets, int i) {
    bullets.erase(bullets.begin() + i);
}

void GameState::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

    render_mesh(renderer,
                transform_mesh(player.mesh, player.position, player.angle),
                camera);

    for (auto astroid : astroids) {
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        render_mesh(
            renderer,
            transform_mesh(astroid.mesh, astroid.position, astroid.angle),
            camera);
    }

    for (auto bullet : bullets) {
        SDL_SetRenderDrawColor(renderer, 0xff, 0x0, 0x0, 0xff);
        render_mesh(renderer, box_mesh(AABB{{bullet.position}, bullet.hitbox}),
                    camera);
    }

    for (auto particle : particles) {
        SDL_SetRenderDrawColor(
            renderer, 0xff * (1 - (particle.timeElapsed / particle.lifetime)),
            0x0, 0x0, 0xff);
        auto idk = world_to_screen(particle.position, camera);
        render_mesh(renderer,
                    box_mesh(AABB{{particle.position},
                                  Vector2{particleSize, particleSize}}),
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
                         SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

        std::cout << "\033[H";
        std::cout << std::format("{}\n", fps);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_SPACE) {
                    gameState.spaceDown = true;
                }
            }
        }

        gameState.update(deltaTime);
        gameState.render(renderer);

        lastFrame = currentFrame;
        gameState.spaceDown = false;
    }

    SDL_Quit();

    return 0;
}
