#pragma once
#include <glm/glm.hpp>
#include <SDL.h>

struct HealthComponent {
public:
	int healthPercentage;

	HealthComponent(int healthPercentage = 1) {
		this->healthPercentage = healthPercentage;
	}
};