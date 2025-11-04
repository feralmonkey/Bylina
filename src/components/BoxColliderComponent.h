#pragma once

#include <glm/glm.hpp>

struct BoxColliderComponent {
	glm::vec2 offset{};
	glm::vec2 size{};

	explicit BoxColliderComponent(const int w = 8, const int h = 8, const glm::vec2 offset = glm::vec2(0))
	{
		this->size = { w, h };
		this->offset = offset;
	}
};
