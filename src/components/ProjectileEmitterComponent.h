#pragma once
#include <glm/glm.hpp>
#include <SDL.h>

struct ProjectileEmitterComponent {
	public:
		glm::vec2 projectileVelocity;
		int repeatFrequency;
		int projectileDuration;
		int hitPercentDamage;
		int lastEmissionTime;
		bool isFriendly;

		ProjectileEmitterComponent(glm::vec2 projectileVelocity = glm::vec2(0), int repeatFrequency = 0, int projectileDuration = 10000, int hitPercentDamage = 10, bool isFriendly = false) {
			this->projectileVelocity = projectileVelocity;
			this->projectileDuration = projectileDuration;
			this->repeatFrequency = repeatFrequency;
			this->hitPercentDamage = hitPercentDamage;
			this->isFriendly = isFriendly;
			this->lastEmissionTime = SDL_GetTicks();
		}
};