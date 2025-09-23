#pragma once

# include <glm/glm.hpp>

struct RigidBodyComponent {
	glm::vec2 velocity;
	glm::vec2 lastMoveDir{ 0.0f, 0.0f };
	bool inMotion = false;
	
	RigidBodyComponent(glm::vec2 velocity = glm::vec2(0.0, 0.0), bool inMotion = false) {
		this->velocity = velocity;
		this->inMotion = inMotion;
	}
};
