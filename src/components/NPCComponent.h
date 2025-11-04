#pragma once
#include <unordered_map>
#include "../enums/NPCMovement.h"

struct NPCComponent {
	int framesSinceLastAction = 0;
	std::string name;
	std::unordered_map<std::string, std::string> conversation;
	MovementPattern movementPattern;
	MovementSpeed speed;

	NPCComponent(const std::string& name, const std::unordered_map<std::string, std::string>& conversation, const MovementPattern movementPattern = MovementPattern::Still, const MovementSpeed speed = MovementSpeed::Normal)
	{
		this->name = name;
		this->conversation = conversation;
		this->movementPattern = movementPattern;
		this->speed = speed;
	}
};