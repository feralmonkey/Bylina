#pragma once

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

    const float tileSize = 16.0f;
    const float moveSpeed = 50.0f;
    const float snapSpeed = 50.0f;
    const float eps = 0.0001f;  // epsilon - represents tolerance

public:
    MovementSystem(entt::registry& reg, entt::dispatcher& dispatcher) : registry(reg) {}

    void Update(double deltaTime) {
        auto view = registry.view<TransformComponent, RigidBodyComponent>();

        for (auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& rigidBody = view.get<RigidBodyComponent>(entity);

            if (IsMoving(rigidBody)) {
                ApplyMovement(transform, rigidBody, deltaTime);
                AlignOrthogonalAxis(transform, rigidBody, deltaTime);
                UpdateLastMoveDir(rigidBody);
            }
            else {
                SnapToNextTile(transform, rigidBody, deltaTime);
            }
        }
    }

private:
    // returns true if the entity;s rigid body component currently has an active velocity on either axis
    bool IsMoving(const RigidBodyComponent& rigidBody) {
        return rigidBody.velocity.x != 0.0f || rigidBody.velocity.y != 0.0f;
    }

    void ApplyMovement(TransformComponent& transform, const RigidBodyComponent& rigidBody, double deltaTime) {
        glm::vec2 frameMove = rigidBody.velocity * (moveSpeed * static_cast<float>(deltaTime));
        transform.previousPosition = transform.position;
        transform.position += frameMove;
    }

    // keeps sprite aligned on grid - no diagonals
    void AlignOrthogonalAxis(TransformComponent& transform, const RigidBodyComponent& rigidBody, double deltaTime) {
        if (rigidBody.velocity.x != 0.0f) {
            NudgeTowardNearest(transform.position.y, deltaTime);
        }
        else if (rigidBody.velocity.y != 0.0f) {
            NudgeTowardNearest(transform.position.x, deltaTime);
        }
    }

    void NudgeTowardNearest(float& pos, double deltaTime) {
        float row = std::floor(pos / tileSize);
        float remainder = pos - row * tileSize;
        if (remainder < eps) return;

        float target = (remainder < tileSize / 2.0f) ? row * tileSize : (row + 1) * tileSize;
        float diff = target - pos;
        float step = std::clamp(diff, -snapSpeed * static_cast<float>(deltaTime), snapSpeed * static_cast<float>(deltaTime));
        pos += step;
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
        float cell = std::floor(pos / tileSize);
        float remainder = pos - cell * tileSize;

        if (std::abs(remainder) < eps) {
            dir = 0.0f; // aligned, done
            return;
        }

        float target = (dir > 0.0f) ? (cell + 1) * tileSize : cell * tileSize;
        float diff = target - pos;
        float step = std::clamp(diff, -snapSpeed * static_cast<float>(deltaTime), snapSpeed * static_cast<float>(deltaTime));
        pos += step;

        if (std::abs(target - pos) < eps) {
            pos = target;
            dir = 0.0f; // finished snapping
        }
    }
};
