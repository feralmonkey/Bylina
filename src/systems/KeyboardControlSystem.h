#pragma once
#include <SDL.h>
#include "entt.hpp"
#include <spdlog/spdlog.h>
#include "../components/KeyboardControlComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/PlayerComponent.h"
#include "../events/KeyPressedEvent.h"


inline void KeyboardControlSystem(const SDL_Event& e, entt::registry& reg) {
	
	auto view = reg.view<PlayerComponent, RigidBodyComponent, SpriteComponent>();
	entt::entity player = view.front();
	
	RigidBodyComponent& rigidBody = view.get<RigidBodyComponent>(player);
	SpriteComponent& sprite = view.get<SpriteComponent>(player);

	if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.scancode) {
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
	else if (e.type == SDL_KEYUP) {
		switch (e.key.keysym.scancode) {
		case SDL_SCANCODE_A:
		case SDL_SCANCODE_LEFT:
		case SDL_SCANCODE_D:
		case SDL_SCANCODE_RIGHT: rigidBody.velocity.x = 0; break;
		case SDL_SCANCODE_W:
		case SDL_SCANCODE_UP:
		case SDL_SCANCODE_S:
		case SDL_SCANCODE_DOWN:  rigidBody.velocity.y = 0; break;
		default: break;
		}
	}
}