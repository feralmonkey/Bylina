#pragma once

#include <glm/glm.hpp>

struct BoxColliderComponent {
	glm::vec2 offset;
	glm::vec2 size;

	BoxColliderComponent(int w = 8, int h = 8, glm::vec2 offset = glm::vec2(0)) {
		this->size = { w, h };
		this->offset = offset;
	}
};
