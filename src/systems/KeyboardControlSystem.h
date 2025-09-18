#pragma once
#include <SDL.h>
#include "entt.hpp"
#include <spdlog/spdlog.h>
#include "../components/KeyboardControlComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/PlayerComponent.h"
#include "../components/TransformComponent.h"
#include "../components/TagComponents.h"
#include "../events/KeyPressedEvent.h"


class KeyboardControlSystem {

private:
	entt::registry& reg;

public:
	KeyboardControlSystem(entt::registry& reg, entt::dispatcher& dispatcher)
		: reg(reg) {
		dispatcher.sink<KeyPressedEvent>()
			.connect<&KeyboardControlSystem::onKeyPress>(*this);
	}

	void onKeyPress(const KeyPressedEvent& e) {
		// only one entity should have the player component so just grab the first one and get the required components
		auto view = reg.view<PlayerTag, RigidBodyComponent, SpriteComponent, TransformComponent>();
		entt::entity player = view.front();

		const TransformComponent transform = view.get<TransformComponent>(player);
		RigidBodyComponent& rigidBody = view.get<RigidBodyComponent>(player);
		SpriteComponent& sprite = view.get<SpriteComponent>(player);

		static SDL_Scancode activeKey = SDL_SCANCODE_UNKNOWN;

		if (e.event.type == SDL_KEYDOWN) {
			if (activeKey == SDL_SCANCODE_UNKNOWN) {
				activeKey = e.event.key.keysym.scancode;
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
		/*else if (e.event.type == SDL_KEYUP) {*/
		else {
			activeKey = SDL_SCANCODE_UNKNOWN;
			rigidBody.velocity.x = 0;
			rigidBody.velocity.y = 0;
		}
	}
};