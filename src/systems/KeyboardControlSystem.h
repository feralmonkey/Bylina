#pragma once
#include "../ecs/ecs.h"
#include "../events/EventBus.h"
#include "../events/KeyPressedEvent.h"
#include <spdlog/spdlog.h>
#include "../components/RigidBodyComponent.h"
#include "../components/KeyboardControlComponent.h"
#include "../components/SpriteComponent.h"


class KeyboardControlSystem : public System {
	public:
		KeyboardControlSystem() {
			RequireComponent<KeyboardControlComponent>();
			RequireComponent<SpriteComponent>();
			RequireComponent<RigidBodyComponent>();
		}

		void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
			eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPress);
		}

		void OnKeyPress(KeyPressedEvent& event) {
			for (auto entity : GetSystemEntities()) {
				const auto& keyboardControl = entity.GetComponent<KeyboardControlComponent>();
				auto& sprite = entity.GetComponent<SpriteComponent>();
				auto& rigidBody = entity.GetComponent < RigidBodyComponent>();

				switch (event.symbol) {
					case SDLK_UP:
					case SDLK_w:
						rigidBody.velocity = keyboardControl.upVelocity;
						sprite.srcRect.y = sprite.height * 0;
						break;
					case SDLK_RIGHT:
					case SDLK_d:
						rigidBody.velocity = keyboardControl.rightVelocity;
						sprite.srcRect.y = sprite.height * 1;
						break;
					case SDLK_DOWN:
					case SDLK_s:
						rigidBody.velocity = keyboardControl.downVelocity;
						sprite.srcRect.y = sprite.height * 2;
						break;
					case SDLK_LEFT:
					case SDLK_a:
						rigidBody.velocity = keyboardControl.leftVelocity;
						sprite.srcRect.y = sprite.height * 3;
						break;
				}
			}
		}

		void Update(char keyPressed, std::unique_ptr<EventBus>& eventBus) {
		}
};