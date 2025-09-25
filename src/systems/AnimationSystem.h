#pragma once

#include <SDL.h>
#include "entt.hpp"
#include <spdlog/spdlog.h>
#include "../components/AnimationComponent.h"
#include "../components/SpriteComponent.h"

inline void AnimationSystem(entt::registry& reg) {
	auto view = reg.view<AnimationComponent, SpriteComponent>();
	for (auto entity : view) {
		auto& animation = view.get<AnimationComponent>(entity);
		auto& sprite = view.get<SpriteComponent>(entity);

		// todo rigolo - for debuggin
		if (sprite.assetId == "npc-tiles") {
			//std::cout << " Render Sprite ";
			int i = 14;
		}

		animation.currentFrame = (
			(SDL_GetTicks() - animation.startTime)
			* animation.frameSpeedRate / 1000) % animation.numFrames;
		sprite.srcRect.x = sprite.anchor_x + (animation.currentFrame * sprite.width); // todo rigolo 0 this is the problem line

		// todo rigolo - for debuggin
		int j = 65;
	}
}
