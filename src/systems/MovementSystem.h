#pragma once

#include "../events/EventBus.h"
#include "../events/CollisionEvent.h"
#include "../components/KeyboardControlComponent.h"
#include "../components/SpriteComponent.h"
#include <SDL.h>
# include <spdlog/spdlog.h> 


inline void MovementSystem(entt::registry& reg) {
	auto view = reg.view<KeyboardControlComponent, SpriteComponent, RigidBodyComponent>();
}

/* TODO RIGOLO - This will not work without an event system in place
class MovementSystem: public System {
	public:
		MovementSystem() {
			RequireComponent<TransformComponent>();
			RequireComponent<RigidBodyComponent>();
		}

		void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
			eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
		}

		void OnCollision(CollisionEvent& event) {
			Entity a = event.a;
			Entity b = event.b; 

			// a projectile hits the player
			if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
				OnEnemyHitObstacle(a, b); // a is the projectile and b is the target
				return;
			}
			if (a.BelongsToGroup("obstacles") && b.BelongsToGroup("enemies")) {
				OnEnemyHitObstacle(a, b); // a is the projectile and b is the target
				return;
			}
		}

		void OnEnemyHitObstacle(Entity enemy, Entity obstacle) {
			if (!enemy.HasComponent<RigidBodyComponent>()) {
				return;
			}
			if (!enemy.HasComponent<SpriteComponent>()) {
				return;
			}

			auto& rigidBody = enemy.GetComponent<RigidBodyComponent>();
			auto& sprite = enemy.GetComponent<SpriteComponent>();

			if (rigidBody.velocity.x != 0) {
				rigidBody.velocity.x *= -1;
				sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
			}

			if (rigidBody.velocity.y != 0) {
				rigidBody.velocity.y *= -1;
				sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
			}
		}

		void Update(double deltaTime) {
			// loop all enteties that the system is interested in
			for (auto entity : GetSystemEntities()) {
				// update entity position based on it's velocity
				TransformComponent& transform = entity.GetComponent<TransformComponent>();
				RigidBodyComponent rigidBody = entity.GetComponent<RigidBodyComponent>();
				SpriteComponent sprite = entity.GetComponent<SpriteComponent>();

				transform.position.x += rigidBody.velocity.x * deltaTime;
				transform.position.y += rigidBody.velocity.y * deltaTime;

				if (entity.HasTag("player")) {
					int paddingLeft = 10;
					int paddingTop = 10;
					int paddingRight = 50;
					int paddingBottom = 50;

					transform.position.x = transform.position.x < paddingLeft ? paddingLeft : transform.position.x;
					transform.position.x = transform.position.x > Game::mapWidth - paddingRight ? Game::mapWidth - paddingRight : transform.position.x;
					transform.position.y = transform.position.y < paddingTop ? paddingTop : transform.position.y;
					transform.position.y = transform.position.y > Game::mapHeight - paddingBottom ? Game::mapHeight - paddingBottom : transform.position.y;
				}

				// kill non-player entity if it moves outside the map
				bool isEntityOutsideMap = (
					transform.position.x < 0 ||
					transform.position.x > Game::mapWidth ||
					transform.position.y < 0 ||
					transform.position.y > Game::mapHeight
				);

				// if entity is not a player - kill it and exit function
				if (isEntityOutsideMap && !entity.HasTag("player")) {
					entity.Kill();
					return;
				}

				//this->upVelocity = upVelocity;
				//this->rightVelocity = rightVelocity;
				//this->downVelocity = downVelocity;
				//this->leftVelocity = leftVelocity;
			}
		}
};
*/