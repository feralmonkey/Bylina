#pragma once

#include <SDL.h>
#include "entt.hpp"
#include <spdlog/spdlog.h>
#include "../components/BoxColliderComponent.h"
#include "../components/TransformComponent.h"

inline void RenderColliderSystem(entt::registry& reg, SDL_Renderer* renderer, SDL_Rect camera) {
	auto view = reg.view<BoxColliderComponent, TransformComponent>();

	for (auto entity : view) {
		const auto& transform = view.get<TransformComponent>(entity);
		const auto& collider = view.get<BoxColliderComponent>(entity);

		SDL_SetRenderDrawColor(renderer, 150, 0, 150, 255); // Red color
		SDL_Rect collisionRect = {
			transform.position.x + collider.offset.x - camera.x,
			transform.position.y + collider.offset.y - camera.y,
			collider.width * transform.scale.x,
			collider.height * transform.scale.y };
		SDL_RenderDrawRect(renderer, &collisionRect);
	}
}