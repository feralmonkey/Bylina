#pragma once

#include <algorithm>
#include <cmath>
#include <entt.hpp>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include "../Constants.h"
#include "../components/KeyboardControlComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/TransformComponent.h"
#include "../enums/Direction.h"
#include "../events/CollisionEvent.h"
#include "../systems/ISystem.h"


class MovementSystem : public ISystem {
private:
    entt::registry& registry;
    entt::dispatcher& dispatcher;
    //const float snapSpeed = 50.0f;
    //const float eps = 0.0001f;  // epsilon - represents tolerance

    /*
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
    */

public:
    MovementSystem(entt::registry& reg, entt::dispatcher& dis)
    : registry(reg), dispatcher(dis) {}

    void Update(double deltaTime) override {
        auto view = registry.view<TransformComponent, RigidBodyComponent>();

        for (auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& rigidBody = view.get<RigidBodyComponent>(entity);

            // store previous position for collision rollback
            transform.previousPosition = transform.position;

            // only move if entity is in motion
            if (rigidBody.inMotion) {
                // old
                //ApplyMovement(transform, rigidBody, deltaTime);
                //CheckTileAlignment(transform, rigidBody);
                //UpdateLastMoveDir(rigidBody);
                float moveSpeed = TILE_SIZE * PC_MOVE_SPEED; // pixels per second
                float moveDistance = moveSpeed * static_cast<float>(deltaTime);

                // Calculate direction vector
                glm::vec2 direction = glm::normalize(transform.nextPosition - transform.position);
                glm::vec2 movement = direction * moveDistance;

                // Move towards target
                transform.position += movement;

                // Update sprite direction
                if (registry.any_of<SpriteComponent>(entity)) {
                    auto& sprite = registry.get<SpriteComponent>(entity);
                    UpdateSpriteDirection(sprite, rigidBody.direction);
                }

                // Check if we've reached the destination
                float distanceRemaining = glm::distance(transform.position, transform.nextPosition);
                if (distanceRemaining <= moveDistance || distanceRemaining < 0.5f) {
                    // Snap to exact tile position
                    transform.position = transform.nextPosition;
                    rigidBody.inMotion = false;
                    rigidBody.velocity = glm::vec2(0.0f, 0.0f);
                }

            }
        }
    }

private:
    static void UpdateSpriteDirection(SpriteComponent& sprite, Direction direction) {
        // Update sprite row based on direction (NES Ultima style)
        switch (direction) {
            case Direction::Up:
                sprite.srcRect.y = sprite.height * 0;
                break;
            case Direction::Right:
                sprite.srcRect.y = sprite.height * 1;
                break;
            case Direction::Down:
                sprite.srcRect.y = sprite.height * 2;
                break;
            case Direction::Left:
                sprite.srcRect.y = sprite.height * 3;
                break;
        }
    }
};
