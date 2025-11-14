#pragma once
#include <string>

struct TextComponent {
	// width and height in characters (each character is 8 pixels)
	int width;
	int height;
	int xOffset;
	int yOffset;

	std::vector<std::vector<std::string>> pages; // each page = vector of lines
	int currentPage = 0;

	// todo rigolo - can probably deprecate these later
	std::string text;
	bool hasSelector;


	explicit TextComponent(const std::string& t, const int w, const int h, const int x, const int y)
	: text(t), width(w), height(h), xOffset(x), yOffset(y) {}

	// old constructor
	/*
	explicit TextComponent(const std::string& text = "", const int width = 8, const int height = 6, const int xOffset = 0, const int yOffset = 0, const bool hasSelector = true) {
		this->text = text;
		this->width = width;
		this->height = height;
		this->xOffset = xOffset;
		this->yOffset = yOffset;
		this->hasSelector = hasSelector;
	}
	*/
};
