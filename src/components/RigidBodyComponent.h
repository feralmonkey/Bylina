#pragma once

#include <glm/glm.hpp>
#include "../enums/Direction.h"

struct RigidBodyComponent {
	glm::vec2 velocity{};
	glm::vec2 lastMoveDir{ 0.0f, 0.0f };
	Direction direction = Direction::Up;
	bool inMotion = false;
	
	explicit RigidBodyComponent(const glm::vec2 velocity = glm::vec2(0.0, 0.0), const bool inMotion = false) {
		this->velocity = velocity;
		this->inMotion = inMotion;
	}
};
