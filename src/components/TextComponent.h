#pragma once
#include <string>
#include <glm/glm.hpp>
#include <SDL.h>

struct TextComponent {
	// width and height in characters (each character is 8 pixels)
	int width;
	int height;
	int xOffset;
	int yOffset;

	std::string text;
	bool hasSelector;

	TextComponent(std::string text = "", int width = 8, int height = 6, int xOffset = 0, int yOffset = 0, bool hasSelector = true) {
		this->text = text;
		this->width = width;
		this->height = height;
		this->xOffset = xOffset;
		this->yOffset = yOffset;
		this->hasSelector = hasSelector;
	}
};
