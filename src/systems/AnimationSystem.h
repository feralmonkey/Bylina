#pragma once

#include <SDL.h>
#include "../components/AnimationComponent.h"
#include "../components/SpriteComponent.h"
#include <spdlog/spdlog.h> 

class AnimationSystem : public System {
	public:
		//AnimationSystem() {
		//	RequireComponent<AnimationComponent>();
		//	RequireComponent<SpriteComponent>();
		//}

		//void Update() {
		//	for (auto entity : GetSystemEntities()) {
		//		auto& animation = entity.GetComponent<AnimationComponent>();
		//		auto& sprite = entity.GetComponent<SpriteComponent>();

		//		animation.currentFrame = (
		//			(SDL_GetTicks() - animation.startTime) 
		//			* animation.frameSpeedRate / 1000) % animation.numFrames;
		//		sprite.srcRect.x = animation.currentFrame * sprite.width;
		//	}
		//}

		inline void Update(entt::registry& reg) {
			auto view = reg.view<AnimationComponent, SpriteComponent>();
			for (auto e : view) {
				auto& animation = e.GetComponent<AnimationComponent>();
				auto& sprite = e.GetComponent<SpriteComponent>();

				animation.currentFrame = (
					(SDL_GetTicks() - animation.startTime)
					* animation.frameSpeedRate / 1000) % animation.numFrames;
				sprite.srcRect.x = animation.currentFrame * sprite.width;
			}
		}
};
