#pragma once

#include "../Constants.h"
#include "../events/CollisionEvent.h"
#include "../systems/ISystem.h"
#include "../components/KeyboardControlComponent.h"
#include "../components/SpriteComponent.h"
#include <SDL.h>
#include <spdlog/spdlog.h> 
#include <entt.hpp>
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>

class MovementSystem {
private:
    entt::registry& registry;
    const float snapSpeed = 50.0f;
    const float eps = 0.0001f;  // epsilon - represents tolerance

    void ApplyMovement(TransformComponent& transform, const RigidBodyComponent& rigidBody, double deltaTime) {
        glm::vec2 frameMove = rigidBody.velocity * (PC_MOVE_SPEED * static_cast<float>(deltaTime));
        transform.previousPosition = transform.position;
        transform.position += frameMove;
    }

    void CheckTileAlignment(TransformComponent& transform, RigidBodyComponent& rigidBody) {
        if (rigidBody.velocity.x > 0) {
            if (transform.position.x >= transform.nextPosition.x) { rigidBody.inMotion = false; transform.position.x = transform.nextPosition.x; }
        } 
        else if (rigidBody.velocity.x < 0) {
            if (transform.position.x <= transform.nextPosition.x) { rigidBody.inMotion = false; transform.position.x = transform.nextPosition.x; }
        }
        else if (rigidBody.velocity.y > 0) {
            if (transform.position.y >= transform.nextPosition.y) { rigidBody.inMotion = false; transform.position.y = transform.nextPosition.y; }
        }
        else if (rigidBody.velocity.y < 0) {
            if (transform.position.y <= transform.nextPosition.y) { rigidBody.inMotion = false; transform.position.y = transform.nextPosition.y; }
        }
    }

    void UpdateLastMoveDir(RigidBodyComponent& rigidBody) {
        rigidBody.lastMoveDir = {
            (rigidBody.velocity.x > 0.0f) ? 1.0f : (rigidBody.velocity.x < 0.0f ? -1.0f : 0.0f),
            (rigidBody.velocity.y > 0.0f) ? 1.0f : (rigidBody.velocity.y < 0.0f ? -1.0f : 0.0f)
        };
    }

    void SnapToNextTile(TransformComponent& transform, RigidBodyComponent& rigidBody, double deltaTime) {
        glm::vec2 dir = rigidBody.lastMoveDir;
        if (std::abs(dir.x) > 0.5f) {
            SnapAxis(transform.position.x, dir.x, deltaTime);
        }
        else if (std::abs(dir.y) > 0.5f) {
            SnapAxis(transform.position.y, dir.y, deltaTime);
        }
    }

    void SnapAxis(float& pos, float dir, double deltaTime) {
        float cell = std::floor(pos / TILE_SIZE);
        float remainder = pos - cell * TILE_SIZE;

        if (std::abs(remainder) < eps) {
            dir = 0.0f; // aligned, done
            return;
        }

        float target = (dir > 0.0f) ? (cell + 1) * TILE_SIZE : cell * TILE_SIZE;
        float diff = target - pos;
        float step = std::clamp(diff, -snapSpeed * static_cast<float>(deltaTime), snapSpeed * static_cast<float>(deltaTime));
        pos += step;

        if (std::abs(target - pos) < eps) {
            pos = target;
            dir = 0.0f; // finished snapping
        }
    }

public:
    MovementSystem(entt::registry& reg, entt::dispatcher& dispatcher) : registry(reg) {}

    void Update(double deltaTime) {
        auto view = registry.view<TransformComponent, RigidBodyComponent>();

        for (auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& rigidBody = view.get<RigidBodyComponent>(entity);

            if (rigidBody.inMotion) {
                ApplyMovement(transform, rigidBody, deltaTime);
                CheckTileAlignment(transform, rigidBody);
                UpdateLastMoveDir(rigidBody);
            }
        }
    }
};
