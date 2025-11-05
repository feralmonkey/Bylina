#pragma once
#include <iostream>
#include <SDL.h>
#include "entt.hpp"
#include <spdlog/spdlog.h>
#include "../Constants.h"
#include "../systems/RenderTextSystem.h"
#include "../components/KeyboardControlComponent.h"
#include "../components/MenuComponent.h"
#include "../components/PlayerComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/TagComponents.h"
#include "../components/TextComponent.h"
#include "../components/TransformComponent.h"
#include "../events/KeyPressedEvent.h"
#include "../events/KeyUpEvent.h"
#include "../events/MenuOpenEvent.h"
#include "../events/MenuCloseEvent.h"
#include "../events/MenuNavigateEvent.h"
#include "../events/TalkEvent.h"
#include "../enums/Direction.h"
#include "../enums/InputState.h"


class KeyboardControlSystem final : public ISystem {

private:
	entt::registry& reg;
	entt::dispatcher& dispatcher;
	std::vector<InputState>& inputStack;
	RenderTextSystem& textSystem;
	InputState currentInput;
	//SDL_Scancode activeKey = SDL_SCANCODE_UNKNOWN;

	// track which movement keys are currently held
	bool upHeld = false;
	bool downHeld = false;
	bool leftHeld = false;
	bool rightHeld = false;

	void ClearOld() const {
		auto text_view = reg.view<TextComponent>();
		for (auto entity : text_view) {
			reg.destroy(entity);
		}
	}

	// Simplified - just set movement intent
	void StartMovement(RigidBodyComponent& rigidBody, TransformComponent& transform, int x, int y, Direction direction) {
		// Only start new movement if not already moving
		if (rigidBody.inMotion) {
			return;
		}

		rigidBody.velocity = glm::vec2(x, y);
		rigidBody.direction = direction;
		rigidBody.lastMoveDir = glm::vec2(static_cast<float>(x), static_cast<float>(y));
		transform.nextPosition = transform.position + rigidBody.velocity * TILE_SIZE;
		rigidBody.inMotion = true;
	}

	/*
	// an almost identical copy of this method is also in the NPC system - it should move to the MovementSystem at some point
	void MoveSprite(RigidBodyComponent& rigidBody, SpriteComponent& sprite, TransformComponent& transform, int x, int y, int spriteIndex, Direction direction) {
		// todo rigolo - check for collision here
		rigidBody.velocity = { x, y };
		transform.nextPosition = transform.position + rigidBody.velocity * TILE_SIZE;
		rigidBody.lastMoveDir = { static_cast<float>(x), static_cast<float>(y) };
		sprite.srcRect.y = sprite.height * spriteIndex;
		rigidBody.direction = direction;
		rigidBody.inMotion = true;
	}
*/
	void PlayerControl(const KeyPressedEvent& e) {
		auto view = reg.view<PlayerComponent, RigidBodyComponent, SpriteComponent, TransformComponent>();
		entt::entity entity = view.front();

		TransformComponent& transform = view.get<TransformComponent>(entity);
		RigidBodyComponent& rigidBody = view.get<RigidBodyComponent>(entity);
		//SpriteComponent& sprite = view.get<SpriteComponent>(entity);
		//PlayerComponent& player = view.get<PlayerComponent>(entity);

		
		if (e.event.type == SDL_KEYDOWN) {
			//if (rigidBody.inMotion == true) { return; } // -- todo rigolo GOOD IDEA! But broken. ( it might not be - collision may be broken)

			//if (activeKey == SDL_SCANCODE_UNKNOWN) {
			//		activeKey = e.event.key.keysym.scancode;
			//}

			switch (e.event.key.keysym.scancode) {
			case SDL_SCANCODE_W:
			case SDL_SCANCODE_UP: {
				upHeld = true;
				StartMovement(rigidBody, transform, 0, -1, Direction::Up);
				break;
			}
			case SDL_SCANCODE_D:
			case SDL_SCANCODE_RIGHT: {
				rightHeld = true;
				StartMovement(rigidBody, transform, 1, 0, Direction::Right);
				break;
			}
			case SDL_SCANCODE_S:
			case SDL_SCANCODE_DOWN: {
				downHeld = true;
				StartMovement(rigidBody, transform, 0, 1, Direction::Down);
				break;
			}
			case SDL_SCANCODE_A:
			case SDL_SCANCODE_LEFT: {
				leftHeld = true;
				StartMovement(rigidBody, transform, -1, 0, Direction::Left);
				break;
			}
			case SDL_SCANCODE_X: {
				spdlog::info("action button pressed - opening menu");
				inputStack.push_back(InputState::MenuControl);
				
				// Create a menu entity
				entt::entity menu = reg.create();

				reg.emplace<MenuComponent>(menu,
					std::vector<std::string>{"Talk", "Cast", "Use", "Search", "Status", "Equip", "Order"},
					0,
					true
				);
				dispatcher.enqueue<MenuOpenEvent>();
				break;
			}
			case SDL_SCANCODE_Z: {
				spdlog::info("cancel button pressed");
				textSystem.ClearTextBox();
				break;
			}
			default: break;
			}
		}
		//else {
		//	activeKey = SDL_SCANCODE_UNKNOWN;
		//}
	}

	void MenuControl(const KeyPressedEvent& e) const {
		auto view = reg.view<MenuComponent>();
		for (auto const entity : view) {
			auto& menu = view.get<MenuComponent>(entity);

			if (!menu.isActive) return;

			if (e.event.type == SDL_KEYDOWN) {
				switch (e.event.key.keysym.scancode) {
				case SDL_SCANCODE_UP:
					textSystem.ClearText();
					menu.currentIndex = (menu.currentIndex - 1 + static_cast<int>(menu.options.size())) % static_cast<int>(menu.options.size());
					std::cout << menu.currentIndex << std::endl;
					dispatcher.enqueue<MenuNavigateEvent>();
					break;
				case SDL_SCANCODE_DOWN:
					textSystem.ClearText();
					menu.currentIndex = (menu.currentIndex + 1) % static_cast<int>(menu.options.size());
					std::cout << menu.currentIndex << std::endl;
					dispatcher.enqueue<MenuNavigateEvent>();
					break;
				case SDL_SCANCODE_X: 
					// confirm
					spdlog::info("Selected: {}", menu.options[menu.currentIndex]);
					if (menu.options[menu.currentIndex] == "Talk") {
						auto inner_view = reg.view<PlayerComponent, TransformComponent, RigidBodyComponent>();
						const auto inner_entity = inner_view.front();
						//const auto& player = inner_view.get<PlayerComponent>(inner_entity);
						auto& rigidBody = inner_view.get<RigidBodyComponent>(inner_entity);
						const auto& transform = inner_view.get<TransformComponent>(inner_entity);

						dispatcher.enqueue<TalkEvent>(transform.position, rigidBody.direction);
					}
					break;
				case SDL_SCANCODE_Z: 
					// cancel
					menu.isActive = false;
					dispatcher.enqueue<MenuCloseEvent>();
						break;
				default:
					break;
				}
			}
		}
	}

public:
	KeyboardControlSystem(entt::registry& reg, entt::dispatcher& dispatcher, std::vector<InputState>& inputStack, RenderTextSystem& textSystem)
		: reg(reg), dispatcher(dispatcher), inputStack(inputStack), textSystem(textSystem) {

		// set player control on the stack during construction
		inputStack.push_back(InputState::PlayerControl);
		currentInput = InputState::PlayerControl;

		dispatcher.sink<KeyPressedEvent>()
			.connect<&KeyboardControlSystem::onKeyPress>(*this);
		dispatcher.sink<KeyUpEvent>()
			.connect<&KeyboardControlSystem::onKeyUp>(*this);
	}

	void Update(double deltaTime) override {
		// Check if player finished moving and a key is still held
		auto view = reg.view<PlayerComponent, RigidBodyComponent, TransformComponent>();

		entt::entity player = view.front();
		RigidBodyComponent& rigidBody = view.get<RigidBodyComponent>(player);
		TransformComponent& transform = view.get<TransformComponent>(player);

		// If not moving and a key is held, start moving again
		if (!rigidBody.inMotion) {
			if (upHeld) {
				StartMovement(rigidBody, transform, 0, -1, Direction::Up);
			} else if (rightHeld) {
				StartMovement(rigidBody, transform, 1, 0, Direction::Right);
			} else if (downHeld) {
				StartMovement(rigidBody, transform, 0, 1, Direction::Down);
			} else if (leftHeld) {
				StartMovement(rigidBody, transform, -1, 0, Direction::Left);
			}
		}
	}

	/*
	void onKeyUp(const KeyUpEvent& e) {
		auto view = reg.view<PlayerComponent, RigidBodyComponent, SpriteComponent, TransformComponent>();
		const entt::entity player = view.front();

		RigidBodyComponent& rigidBody = view.get<RigidBodyComponent>(player);


		if (e.event.type != SDL_KEYDOWN) {
			activeKey = SDL_SCANCODE_UNKNOWN;
		}
	}
*/
	void onKeyUp(const KeyUpEvent& e) {
		// Track when keys are released
		switch (e.event.key.keysym.scancode) {
			case SDL_SCANCODE_W:
			case SDL_SCANCODE_UP:
				upHeld = false;
				break;
			case SDL_SCANCODE_D:
			case SDL_SCANCODE_RIGHT:
				rightHeld = false;
				break;
			case SDL_SCANCODE_S:
			case SDL_SCANCODE_DOWN:
				downHeld = false;
				break;
			case SDL_SCANCODE_A:
			case SDL_SCANCODE_LEFT:
				leftHeld = false;
				break;
			default:
				break;
		}
	}

	void onKeyPress(const KeyPressedEvent& e) {
		// get the current input state
		currentInput = inputStack.back();

		if (currentInput == InputState::PlayerControl) {
			/*std::cout << "PlayerControl" << std::endl;*/
			PlayerControl(e);
			return;
		}
		else if (currentInput == InputState::MenuControl) {
			/*std::cout << "MenuControl" << std::endl;*/
			MenuControl(e);
			return;
		}
		std::cout << "NoControl" << std::endl;
	}
};