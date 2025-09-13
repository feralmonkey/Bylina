#pragma once
# include <glm/glm.hpp> // math library

struct Position {
	int x;
	int y;
};

struct TransformComponent {
	Position position;
	glm::vec2 scale;
	double rotation;

	TransformComponent(glm::vec2 position = glm::vec2(0,0), glm::vec2 scale = glm::vec2(1,1), double rotation = 0.0) {
		this->position.x = (int)position.x;
		this->position.y = (int)position.y;
		this->scale = scale;
		this->rotation = rotation;
	}
};

