#pragma once

#include <glm/glm.hpp>

struct BoxColliderComponent {
	int width;
	int height;
	glm::vec2 offset;

	BoxColliderComponent(int width = 32, int height = 32, glm::vec2 offset = glm::vec2(0)) {
		this->width = width;
		this->height = height;
		this->offset = offset;
	}
};
