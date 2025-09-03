#pragma once
#include <SDL.h>
#include <entt/entt.hpp>
#include <algorithm>
#include <spdlog/spdlog.h>
#include "systems/AnimationSystem.h"
#include "systems/CameraMovementSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/KeyboardControlSystem.h"
#include "systems/MovementSystem.h"
#include "systems/RenderColliderSystem.h"
#include "systems/RenderGUISystem.h"
#include "systems/RenderSystem.h"
#include "systems/RenderTextSystem.h"
#include "systems/ScriptSystem.h"

// NES-like logical resolution
constexpr int LOGICAL_W = 256;
constexpr int LOGICAL_H = 240;


/*
inline void input_system(entt::registry& reg, const Uint8* kb) {
    auto view = reg.view<Player, Velocity>();
    for (auto e : view) {
        auto& v = view.get<Velocity>(e);
        v.dx = v.dy = 0;
        if (kb[SDL_SCANCODE_LEFT])  v.dx = -1;
        if (kb[SDL_SCANCODE_RIGHT]) v.dx = +1;
        if (kb[SDL_SCANCODE_UP])    v.dy = -1;
        if (kb[SDL_SCANCODE_DOWN])  v.dy = +1;
        spdlog::info("input detected");
    }
}

inline void movement_system(entt::registry& reg) {
    auto view = reg.view<Position, Velocity>();
    for (auto e : view) {
        auto& p = view.get<Position>(e); 
        const auto& v = view.get<Velocity>(e);
        p.x += v.dx;
        p.y += v.dy;
        p.x = std::clamp(p.x, 0, LOGICAL_W - 16);
        p.y = std::clamp(p.y, 0, LOGICAL_H - 16);
    }
}

inline void camera_system(entt::registry& reg, SDL_Rect& cam) {
    auto view = reg.view<CameraFollow, Position>();
    for (auto e : view) {
        const auto& p = view.get<Position>(e);
        cam.x = std::clamp(p.x - cam.w / 2, 0, std::max(0, LOGICAL_W - cam.w));
        cam.y = std::clamp(p.y - cam.h / 2, 0, std::max(0, LOGICAL_H - cam.h));
    }
}

inline void render_system(entt::registry& reg, SDL_Renderer* r, const SDL_Rect& cam) {
    // Clear
    SDL_RenderClear(r);

    // Draw a simple checkerboard tile map for now
    const int tile = 16;
    for (int y = 0; y < LOGICAL_H; y += tile) {
        for (int x = 0; x < LOGICAL_W; x += tile) {
            SDL_Rect rect{ x - cam.x, y - cam.y, tile, tile };
            // Alternate fill color by tile parity (no explicit colors in code here; rely on default draw color set externally)
            if (((x + y) / tile) % 2 == 0) {
                SDL_RenderFillRect(r, &rect);
            }
            else {
                SDL_RenderDrawRect(r, &rect);
            }
        }
    }

    // Draw sprites (solid rects for now)
    auto view = reg.view<Position, Sprite>();
    for (auto e : view) {
        const auto& p = view.get<Position>(e);
        const auto& s = view.get<Sprite>(e);
        SDL_Rect rect{ p.x - cam.x, p.y - cam.y, s.w, s.h };
        SDL_RenderFillRect(r, &rect);
    }

    SDL_RenderPresent(r);
}
*/