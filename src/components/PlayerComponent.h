#pragma once
#include <glm/glm.hpp>
#include <SDL.h>
#include <spdlog/spdlog.h>
#include "../enums/Direction.h"

struct PlayerComponent {
	bool isActive = true;
	Direction direction = Direction::Up;
	
	PlayerComponent(bool isActive = true) {
		this->isActive = isActive;
	}
};