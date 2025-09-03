#pragma once
#include "../ecs/ecs.h"
#include "../components/ProjectileEmitterComponent.h"
#include "../components/ProjectileComponent.h"
#include "../components/TransformComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/BoxColliderComponent.h"
#include <SDL.h>

class ProjectileLifecycleSystem : public System {
public:
	ProjectileLifecycleSystem() {
		RequireComponent<ProjectileComponent>();
	}

	void Update() {
		for (auto entity : GetSystemEntities()) {
			auto projectile = entity.GetComponent<ProjectileComponent>();

			// todo kill projectiles after they reach their duration limit
			if (SDL_GetTicks() - projectile.startTime > projectile.duration) {
				entity.Kill();
			}
		}
	}
};
