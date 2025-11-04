#pragma once
#include <string>

struct TextComponent {
	// width and height in characters (each character is 8 pixels)
	int width;
	int height;
	int xOffset;
	int yOffset;

	std::string text;
	bool hasSelector;

	explicit TextComponent(const std::string& text = "", const int width = 8, const int height = 6, const int xOffset = 0, const int yOffset = 0, const bool hasSelector = true) {
		this->text = text;
		this->width = width;
		this->height = height;
		this->xOffset = xOffset;
		this->yOffset = yOffset;
		this->hasSelector = hasSelector;
	}
};
