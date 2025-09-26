#pragma once
#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>
#include "../enums/NPCMovement.h"

struct NPCComponent {
	int framesSinceLastAction = 0;
	std::string name;
	std::unordered_map<std::string, std::string> conversation;
	MovementPattern movementPattern;
	MovementSpeed speed;

	glm::vec2 currentTile{ 0, 0 };  // todo rigolo - not currently using this for anything but could be a big help later
	glm::vec2 targetTile{ 0, 0 };
	bool isMoving = false;

	NPCComponent(std::string name, std::unordered_map<std::string, std::string> conversation, MovementPattern movementPattern = MovementPattern::Still, MovementSpeed speed = MovementSpeed::Normal) 
	{
		this->name = name;
		this->conversation = conversation;
		this->movementPattern = movementPattern;
		this->speed = speed;
	}
};