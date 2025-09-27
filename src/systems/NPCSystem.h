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
#include "../enums/Direction.h"
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

	void MoveSprite(RigidBodyComponent&rigidBody, SpriteComponent& sprite, TransformComponent& transform, int x, int y, int spriteIndex, Direction direction) {
		// todo rigolo - check for collision here
		rigidBody.velocity = { x, y };
		transform.nextPosition = transform.position + rigidBody.velocity * TILE_SIZE;
		rigidBody.lastMoveDir = { (float)x, (float)y };
		sprite.srcRect.y = sprite.height * spriteIndex;
		rigidBody.direction = direction;
	}

	void MoveRandom(RigidBodyComponent& rigidBody, TransformComponent& transform, SpriteComponent& sprite) {

		switch (RNG()) {
		case 6: 
			MoveSprite(rigidBody, sprite, transform, 0, -1, 0, Direction::Up);
			break;  // up
		case 7: 
			MoveSprite(rigidBody, sprite, transform, 1, 0, 1, Direction::Right);
			break;
		case 8: 
			MoveSprite(rigidBody, sprite, transform, 0, 1, 2, Direction::Down);
			break;  // down
		case 9: 
			MoveSprite(rigidBody, sprite, transform, -1, 0, 3, Direction::Left);
			break;  // left
		default: 
			rigidBody.velocity = { 0.0f, 0.0f };
			break;  // idle
		}

		if (rigidBody.velocity != glm::vec2{ 0,0 }) {
			rigidBody.inMotion = true;
			transform.nextPosition = transform.position + rigidBody.velocity * TILE_SIZE;  // todo rigolo - smooth movement has something to do here.... maybe only set motion and handle position in the movment system
		}
		else {
			// todo rigolo - inMotion should be false by default. It should be reset to false in the movement system not here
			//rigidBody.inMotion = false;
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
				MoveRandom(rigidBody, transform, sprite);
			}

			if (!rigidBody.inMotion) continue;


			// todo rigolo - this should really be handled in movement
			//glm::vec2 direction = glm::normalize(transform.nextPosition - transform.position);
			//float speed = 60.0f; // pixels/sec or tie to npc.speed
			//transform.position += direction * TILE_SIZE;// *speed;// *deltaTime;

			////// If we’ve arrived (within epsilon), snap to grid and stop
			//if (transform.position == transform.nextPosition) {
			//	rigidBody.velocity = { 0.0f, 0.0f };
			//	rigidBody.inMotion = false;
			//}
			//if (glm::length(transform.nextPosition - transform.position) < 1.0f) {
			//	transform.position = transform.nextPosition;
			//	rigidBody.inMotion = false;
			//}
		}
	}
};