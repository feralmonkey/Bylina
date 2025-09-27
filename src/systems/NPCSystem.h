#pragma once

#include <iostream>
#include <random>
#include <entt.hpp>
#include <glm/glm.hpp>
#include "../Constants.h"
#include "../components/NPCComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/TransformComponent.h"
#include "../enums/NPCMovement.h"
#include "../events/TalkEvent.h"
#include "../systems/RenderTextSystem.h"


class NPCSystem {
private:
	entt::registry& registry;
	entt::dispatcher& dispatcher;
	RenderTextSystem& textSystem;

	int RNG() {
		std::random_device rd;                     // seed
		std::mt19937 gen(rd());                    // Mersenne Twister engine
		std::uniform_int_distribution<> dist(0, 9); // inclusive range [1,10]

		return dist(gen);
	}

	void onTalk(const TalkEvent& e) { 
		
		auto view = registry.view<NPCComponent, TransformComponent>();
		bool foundTarget = false;
		for (auto entity : view) {
			auto& npc       = view.get<NPCComponent>(entity);
			auto& transform = view.get<TransformComponent>(entity);
			if (e.target == transform.position) {
				textSystem.TextBox(npc.conversation["default"]);
				foundTarget = true;
			}
		}
		if (!foundTarget) {
			textSystem.TextBox("No one to talk to");
		}
	}

	void MoveRandom(RigidBodyComponent& rigidBody, TransformComponent& transform, SpriteComponent& sprite, NPCComponent& npc) {

		glm::vec2 dir{ 0, 0 };

		switch (RNG()) {
		case 6: 
			dir = { 0, -1 }; 
			sprite.srcRect.y = 0;  
			break;  // up
		case 7: 
			dir =  { 1, 0 }; 
			sprite.srcRect.y = 16; 
			break;  // right
		case 8: 
			dir =  { 0, 1 }; 
			sprite.srcRect.y = 32; 
			break;  // down
		case 9: 
			dir = { -1, 0 }; 
			sprite.srcRect.y = 48; 
			break;  // left
		default: 
			dir = { 0, 0 }; 
			break;  // idle
		}

		if (dir != glm::vec2{ 0,0 }) {
			npc.isMoving = true;
			npc.targetTile = transform.position + dir * TILE_SIZE; // move 1 square -- tilesize = 16 : this should be a global constant
		}
		else {
			npc.isMoving = false;
		}
	}

public:

	NPCSystem(entt::registry& reg, entt::dispatcher& dis, RenderTextSystem& textSystem) :
		dispatcher(dis),
		registry(reg),
		textSystem(textSystem)
	{
		dispatcher.sink<TalkEvent>().connect<&NPCSystem::onTalk>(*this); //todo rigolo will need to deal with this
	}

	void Update(double deltaTime) {
		auto view = registry.view<NPCComponent>();
		int i = 0;
		for (entt::entity entity : view) {

			NPCComponent& npc = registry.get<NPCComponent>(entity);
			RigidBodyComponent& rigidBody = registry.get<RigidBodyComponent>(entity);
			TransformComponent& transform = registry.get<TransformComponent>(entity);
			SpriteComponent& sprite = registry.get<SpriteComponent>(entity);

			// todo rigolo -- this is not good, should be actual time and not a solid number
			int frameMax = 120;
			if (npc.speed == MovementSpeed::Fast) {  // todo -- handle fast logic later
				frameMax = 60;
			}
			else if (npc.speed == MovementSpeed::Normal) {
				frameMax = 120;
			}
			else if (npc.speed == MovementSpeed::Slow) { // todo rigolo -- handle slow logic later
				frameMax = 180;
			}

			if (npc.framesSinceLastAction >= frameMax) {
				npc.framesSinceLastAction = 0; // reset turn
			}
			else {
				npc.framesSinceLastAction++;
				continue;
			}

			if (npc.movementPattern == MovementPattern::Random) {
				MoveRandom(rigidBody, transform, sprite, npc);
			}

			if (!npc.isMoving) continue;


			glm::vec2 direction = glm::normalize(npc.targetTile - transform.position);
			float speed = 60.0f; // pixels/sec or tie to npc.speed
			transform.position += direction * TILE_SIZE;// *speed;// *deltaTime;

			//// If we’ve arrived (within epsilon), snap to grid and stop
			if (glm::length(npc.targetTile - transform.position) < 1.0f) {
				transform.position = npc.targetTile;
				npc.isMoving = false;
			}
		}
	}
};