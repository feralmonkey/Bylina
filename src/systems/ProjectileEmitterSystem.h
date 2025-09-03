#pragma once
#include "../ecs/ecs.h"
#include "../components/ProjectileEmitterComponent.h"
#include "../components/ProjectileComponent.h"
#include "../components/TransformComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/BoxColliderComponent.h"
#include "../events/EventBus.h"
#include <spdlog/spdlog.h> 
#include <SDL.h>

class ProjectileEmitterSystem : public System {
	public:
		ProjectileEmitterSystem() {
				RequireComponent<ProjectileEmitterComponent>();
				RequireComponent<TransformComponent>();
		}

		void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
			// subscribe to the event of the space key being pressed
			eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitterSystem::OnKeyPress);
		}

		void Update(std::unique_ptr<Registry>& registry) {
			for (auto entity : GetSystemEntities()) {
				auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
				const auto transform = entity.GetComponent<TransformComponent>();

				// bypass emission logic if repeat frequency is 0
				if (projectileEmitter.repeatFrequency == 0) {
					continue;
				}

				// check to see if it is time to emit a new projectile
				if (SDL_GetTicks() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency) {
					glm::vec2 projectilePosition = transform.position;
					if (entity.HasComponent<SpriteComponent>()) {
						const auto sprite = entity.GetComponent<SpriteComponent>();
						projectilePosition.x += (transform.scale.x * sprite.width / 2);
						projectilePosition.y += (transform.scale.y * sprite.height / 2);
					}

					// add a new projectile entity
					Entity projectile = registry->CreateEntity();
					projectile.Group("projectiles");
					projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
					projectile.AddComponent<RigidBodyComponent>(projectileEmitter.projectileVelocity);
					projectile.AddComponent<SpriteComponent>("bullet-texture", 4, 4, 4);
					projectile.AddComponent<BoxColliderComponent>(4, 4);
					projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage, projectileEmitter.projectileDuration);

					projectileEmitter.lastEmissionTime = SDL_GetTicks();
				}
			}
		}

		void OnKeyPress(KeyPressedEvent& event) {
			if (event.symbol == SDLK_SPACE) {
				spdlog::info("space key pressed");
				for (auto entity : GetSystemEntities()) {
					if (entity.HasComponent<CameraFollowComponent>()) {
						const auto projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
						const auto transform = entity.GetComponent<TransformComponent>();
						const auto rigidBody = entity.GetComponent<RigidBodyComponent>();

						// If parent entity has sprite, start the projectile position in the middle of the entity
						glm::vec2 projectilePosition = transform.position;
						if (entity.HasComponent<SpriteComponent>()) {
							auto sprite = entity.GetComponent<SpriteComponent>();
							projectilePosition.x += (transform.scale.x * sprite.width / 2);
							projectilePosition.y += (transform.scale.y * sprite.height / 2);
						}

						// If parent entity direction is controlled by the keyboard keys, modify the direction of the projectile accordingly
						glm::vec2 projectileVelocity = projectileEmitter.projectileVelocity;
						int directionX = 0;
						int directionY = 0;
						if (rigidBody.velocity.x > 0) directionX = +1;
						if (rigidBody.velocity.x < 0) directionX = -1;
						if (rigidBody.velocity.y > 0) directionY = +1;
						if (rigidBody.velocity.y < 0) directionY = -1;
						projectileVelocity.x = projectileEmitter.projectileVelocity.x * directionX;
						projectileVelocity.y = projectileEmitter.projectileVelocity.y * directionY;

						// Create new projectile entity and add it to the world
						Entity projectile = entity.registry->CreateEntity();
						projectile.Group("projectiles");
						projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
						projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
						projectile.AddComponent<SpriteComponent>("bullet-texture", 4, 4, 4);
						projectile.AddComponent<BoxColliderComponent>(4, 4);
						projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage, projectileEmitter.projectileDuration);
					}
				}
			}
		}
};
