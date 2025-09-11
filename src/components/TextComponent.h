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

	TextComponent(std::string text = "", glm::vec2 position = glm::vec2(0), int width = 8, int height = 6, bool hasSelector = false) {
		this->text = text;
		this->position = position;
		this->width = width;
		this->height = height;
		this->hasSelector = hasSelector;
	}
};
