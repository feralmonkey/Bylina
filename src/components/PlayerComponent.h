#pragma once
#include <glm/glm.hpp>
#include <SDL.h>
#include <spdlog/spdlog.h>

struct PlayerComponent {
	bool isActive = true;

	PlayerComponent(bool isActive = true) {
		this->isActive = isActive;
	}
};