#pragma once

# include <glm/glm.hpp>

struct RigidBodyComponent {
	glm::vec2 velocity;
	bool inMotion = false;

	RigidBodyComponent(glm::vec2 velocity = glm::vec2(0.0, 0.0), bool inMotion = false) {
		this->velocity = velocity;
		this->inMotion = inMotion;
	}
};
