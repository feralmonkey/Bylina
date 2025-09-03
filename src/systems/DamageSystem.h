#pragma once
#include "../ecs/ecs.h"
#include "../components/BoxColliderComponent.h"
#include "../components/ProjectileComponent.h"
#include "../components/HealthComponent.h"
#include "../events/EventBus.h"
#include "../events/CollisionEvent.h"
#include <spdlog/spdlog.h> 

class DamageSystem : public System {
	public:
		DamageSystem() {
			RequireComponent<BoxColliderComponent>();
		}

		void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
			eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
		}

		void OnCollision(CollisionEvent& event) {
			Entity a = event.a;
			Entity b = event.b;

			// a projectile hits the player
			if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
				OnProjectileHitsTarget(a, b, true); // a is the projectile and b is the target
				return;
			}
			if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
				OnProjectileHitsTarget(b, a, true); // b is the projectile and a is the target
				return;
			}

			// a projectile (player or enemy) hits an enemy
			if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
				OnProjectileHitsTarget(a, b); // a is the projectile and b is the target
				return;
			}
			if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
				OnProjectileHitsTarget(b, a); // b is the projectile and a is the target
				return;
			}
		}

		void OnProjectileHitsTarget(Entity projectile, Entity target, bool targetIsPlayer = false) {
			auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

			// exits if the projectile is friendly and the target is the player
			if (projectileComponent.isFriendly && target.HasTag("player")) {
				return;
			}
			// exits if the projectile is not friendly and the target is an enemy
			if (!projectileComponent.isFriendly && target.BelongsToGroup("enemies")) {
				return;
			}

			// damages player
			auto& health = target.GetComponent<HealthComponent>();
			health.healthPercentage -= projectileComponent.hitPercentDamage;

			// kills player if health is 0 or less
			if (health.healthPercentage <= 0) {
				target.Kill();
			}

			// kills projectile
			projectile.Kill();
		}

		void Update() {

		}
};
