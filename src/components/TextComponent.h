#pragma once
#include <string>
#include <glm/glm.hpp>
#include <SDL.h>

struct TextComponent {
	glm::vec2 position;

	// width and height in characters (each character is 8 pixels)
	int width;
	int height;

	std::string text;
	bool hasSelector;

	TextComponent(std::string text = "", glm::vec2 position = glm::vec2(0), bool hasSelector = false) {
		this->position = position;
		this->text = text;
		this->hasSelector = hasSelector;
	}
};
