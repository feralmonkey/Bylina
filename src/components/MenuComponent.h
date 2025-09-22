#pragma once

#include <vector>
#include <string>

struct MenuComponent {
	// menu pointer variables
	std::vector<std::string> options;
	int currentIndex = 0;
	bool isActive = false;
	int itemCount;
};