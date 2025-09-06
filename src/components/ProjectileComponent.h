#pragma once
#include <glm/glm.hpp>
#include <SDL.h>
#include <spdlog/spdlog.h>

struct ProjectileComponent {
	public:
		bool isFriendly;
		int hitPercentDamage;
		int duration;
		int startTime;

		ProjectileComponent(bool isFriendly = false, int hitPercentDamage = 0, int duration = 0) {
			this->isFriendly = isFriendly;
			this->hitPercentDamage = hitPercentDamage;
			this->duration = duration;
			this->startTime = SDL_GetTicks();
		}
		
};