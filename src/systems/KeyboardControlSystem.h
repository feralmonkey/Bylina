#pragma once
#include <SDL.h>
#include "entt.hpp"
#include <spdlog/spdlog.h>
#include "../components/KeyboardControlComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/PlayerComponent.h"
#include "../components/TransformComponent.h"
#include "../events/KeyPressedEvent.h"


inline void KeyboardControlSystem(const SDL_Event& e, entt::registry& reg) {
	
	// only one entity should have the player component so just grab the first one and get the required components
	auto view = reg.view<PlayerComponent, RigidBodyComponent, SpriteComponent, TransformComponent>();
	entt::entity player = view.front();
	
	RigidBodyComponent& rigidBody = view.get<RigidBodyComponent>(player);
	SpriteComponent& sprite = view.get<SpriteComponent>(player);
	const TransformComponent transform = view.get<TransformComponent>(player);

	static SDL_Scancode activeKey = SDL_SCANCODE_UNKNOWN;

	if (e.type == SDL_KEYDOWN) {
		if (activeKey == SDL_SCANCODE_UNKNOWN) {
			activeKey = e.key.keysym.scancode;
		}
		switch (activeKey) {
		case SDL_SCANCODE_A:
		case SDL_SCANCODE_LEFT: {
			rigidBody.velocity.x = -1;
			sprite.srcRect.y = sprite.height * 3;
			break;
		}
		case SDL_SCANCODE_D:
		case SDL_SCANCODE_RIGHT: {
			rigidBody.velocity.x = +1;
			sprite.srcRect.y = sprite.height * 1;
			break;
		}
		case SDL_SCANCODE_W:
		case SDL_SCANCODE_UP: {
			rigidBody.velocity.y = -1;
			sprite.srcRect.y = sprite.height * 0;
			break;
		}
		case SDL_SCANCODE_S:
		case SDL_SCANCODE_DOWN: {
			rigidBody.velocity.y = +1;
			sprite.srcRect.y = sprite.height * 2;
			break;
		}
		default: break;
		}
	}
	/*else if (e.type == SDL_KEYUP) {*/
	else {
		activeKey = SDL_SCANCODE_UNKNOWN;
		rigidBody.velocity.x = 0;
		rigidBody.velocity.y = 0;
		// TODO RIGOLO - SMALL BUG IF A SECOND BUTTON IS PRESSED AND RELEASED THE MOVEMENT WILL STOP
		//if (e.key.keysym.scancode == activeKey) {
		//	activeKey = e.key.keysym.scancode;
		//}

		//switch (activeKey) {
		//case SDL_SCANCODE_A:
		//case SDL_SCANCODE_LEFT:
		//case SDL_SCANCODE_D:
		//case SDL_SCANCODE_RIGHT:rigidBody.velocity.x = 0; break;
		//case SDL_SCANCODE_W:
		//case SDL_SCANCODE_UP:
		//case SDL_SCANCODE_S:
		//case SDL_SCANCODE_DOWN:rigidBody.velocity.y = 0; break;
		//default: break;
		//}
	}
	//spdlog::info("Sprite Transform: " + std::to_string(transform.position.x) + " , " + std::to_string(transform.position.y));
}