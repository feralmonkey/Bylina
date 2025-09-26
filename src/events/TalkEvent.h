#pragma once
#include "entt.hpp"
#include <glm/glm.hpp>
#include "../enums/Direction.h"

struct TalkEvent {
	glm::vec2 target{ -1,-1 };

	TalkEvent(glm::vec2 location, Direction direction) {
		switch (direction) {
		case Direction::Up:
			target = location + glm::vec2{ 0,-16 };
			break;
		case Direction::Right:
			target = location + glm::vec2{ 16,0 };
			break;
		case Direction::Down:
			target = location + glm::vec2{ 0,16 };
			break;
		case Direction::Left:
			target = location + glm::vec2{ -16,0 };
			break;
		default:
			break;
		}
	}
};