#include <entt.hpp>
#include <SDL.h>
#include <sol.hpp>
#include "Game.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <spdlog/spdlog.h>
#include "assetstore/AssetStore.h"
#include "components.hpp"
#include "events/EventBus.h"
#include "systems.hpp"
#include "../src/systems/AnimationSystem.h"
#include "../src/systems/CameraMovementSystem.h"
#include "../src/systems/CollisionSystem.h"
#include "../src/systems/KeyboardControlSystem.h"
#include "../src/systems/MovementSystem.h"
#include "../src/systems/RenderColliderSystem.h"
#include "../src/systems/RenderSystem.h"
#include "../src/systems/RenderTextSystem.h"
#include "../src/systems/ScriptSystem.h"



static void sdl_check(bool ok, const char* msg) {
    if (!ok) { std::cerr << msg << ": " << SDL_GetError() << "\n"; std::exit(1); }
}

int main(int, char**) {
    
    entt::registry registry; // TODO RIGOLO - TEMP! JUST DOING THIS TO CLEAR ERRORS UNTIL I GET A WORKING GAME LOOP IMPLEMENTED

    Game game; // create a game object on the stack - as soon as we leave this function (go out of scope) the game object will be destroyed

    sdl_check(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == 0, "SDL_Init failed");

    // Create window at a nice scale (NES x3)
    int windowScale = 3;
    SDL_Window* win = SDL_CreateWindow(
        "Bylina", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        LOGICAL_W * windowScale, LOGICAL_H * windowScale, SDL_WINDOW_SHOWN);
    sdl_check(win != nullptr, "SDL_CreateWindow failed");

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    sdl_check(ren != nullptr, "SDL_CreateRenderer failed");

    // Set logical size so our drawing uses NES-ish resolution regardless of window size
    SDL_RenderSetLogicalSize(ren, LOGICAL_W, LOGICAL_H);

    // We won't specify explicit colors; use renderer defaults for now

    // Lua
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string);
    spdlog::info("lua state created");

    // Expose a tiny API to Lua
    lua.new_usertype<entt::registry>("Registry");
    lua.new_usertype<Position>("Position",
        sol::constructors<Position(), Position(int, int)>(),
        "x", &Position::x, "y", &Position::y);
    lua.new_usertype<Sprite>("Sprite",
        sol::constructors<Sprite(), Sprite(int, int, int)>(),
        "tileIndex", &Sprite::tileIndex, "w", &Sprite::w, "h", &Sprite::h);

    lua["spawn_player"] = [&](int x, int y) {
        auto e = game.reg.create();
        reg.emplace<Position>(e, x, y);
        reg.emplace<RigidBodyComponent>(e);
        reg.emplace<Sprite>(e, 0, 16, 16);
        reg.emplace<Player>(e);
        reg.emplace<Collider>(e, true);
        reg.emplace<CameraFollow>(e);
        return static_cast<int>(e); 
        };

    try {
        lua.script_file("scripts/init.lua");
        spdlog::info("lua init script loaded");
    }
    catch (const std::exception& e) {
        spdlog::error("error loading lua script");
        std::cerr << e.what() << "\n";
    }

    bool running = true;
    SDL_Event ev;

    // Fixed timestep @ 60 fps
    using clock = std::chrono::steady_clock;
    const auto frame = std::chrono::milliseconds(16);

    // Camera rectangle
    SDL_Rect cam{ 0,0, LOGICAL_W, LOGICAL_H };

    spdlog::info("starting main game loop");
    while (running) {
        auto start = clock::now();

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = false;
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.scancode == SDL_SCANCODE_ESCAPE) running = false;
        }

        const Uint8* kb = SDL_GetKeyboardState(nullptr);

        // Systems
        input_system(reg, kb);
        movement_system(reg);
        camera_system(reg, cam);

        // Render
        render_system(reg, ren, cam);

        // Frame cap
        auto dt = clock::now() - start;
        if (dt < frame) std::this_thread::sleep_for(frame - dt);
    }

    spdlog::info("cleaning up assets and exiting game");
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit(); 
    return 0;
}







