#pragma once

#include <entt.hpp>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include "../Constants.h"
#include "../components/RigidBodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/TransformComponent.h"
#include "../enums/Direction.h"
#include "../systems/ISystem.h"


class MovementSystem : public ISystem {
private:
    entt::registry& registry;
    entt::dispatcher& dispatcher;

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
