#pragma once

#include "../ecs/ecs.h"
#include "../components/TransformComponent.h"
#include "../components/BoxColliderComponent.h"
#include <spdlog/spdlog.h> 
#include <SDL.h>

class RenderColliderSystem : public System {
	public:
		RenderColliderSystem() {
			RequireComponent<BoxColliderComponent>();
			RequireComponent<TransformComponent>();
		}

		void Update(SDL_Renderer * renderer, SDL_Rect camera) {
			for (Entity& entity : GetSystemEntities()) {
				const auto& transform = entity.GetComponent<TransformComponent>();
				const auto& collider = entity.GetComponent<BoxColliderComponent>();

				SDL_SetRenderDrawColor(renderer, 150, 0, 150, 255); // Red color
				SDL_Rect collisionRect = { 
					transform.position.x + collider.offset.x - camera.x, 
					transform.position.y + collider.offset.y - camera.y, 
					collider.width * transform.scale.x, 
					collider.height * transform.scale.y };
				SDL_RenderDrawRect(renderer, &collisionRect);
			}
		}
};