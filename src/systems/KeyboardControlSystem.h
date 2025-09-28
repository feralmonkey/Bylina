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


class KeyboardControlSystem : public ISystem {

private:
	entt::registry& reg;
	entt::dispatcher& dispatcher;
	std::vector<InputState>& inputStack;
	RenderTextSystem& textSystem;
	InputState currentInput;
	SDL_Scancode activeKey = SDL_SCANCODE_UNKNOWN;

	void ClearOld() {
		auto text_view = reg.view<TextComponent>();
		for (auto entity : text_view) {
			reg.destroy(entity);
		}
	}

	// an almost identical copy of this method is also in the NPC system - it should move to the MovementSystem at some point
	void MoveSprite(RigidBodyComponent& rigidBody, SpriteComponent& sprite, TransformComponent& transform, int x, int y, int spriteIndex, Direction direction) {
		// todo rigolo - check for collision here
		rigidBody.velocity = { x, y };
		transform.nextPosition = transform.position + rigidBody.velocity * TILE_SIZE;
		rigidBody.lastMoveDir = { (float)x, (float)y };
		sprite.srcRect.y = sprite.height * spriteIndex;
		rigidBody.direction = direction;
		rigidBody.inMotion = true;
	}

	void PlayerControl(const KeyPressedEvent& e) {
		auto view = reg.view<PlayerComponent, RigidBodyComponent, SpriteComponent, TransformComponent>();
		entt::entity entity = view.front();

		TransformComponent& transform = view.get<TransformComponent>(entity);
		RigidBodyComponent& rigidBody = view.get<RigidBodyComponent>(entity);
		SpriteComponent& sprite = view.get<SpriteComponent>(entity);
		PlayerComponent& player = view.get<PlayerComponent>(entity);

		
		if (e.event.type == SDL_KEYDOWN) {
			if (rigidBody.inMotion == true) { return; } // -- todo rigolo GOOD IDEA! But broken. ( it might not be - collision may be broken)

			if (activeKey == SDL_SCANCODE_UNKNOWN) {
				activeKey = e.event.key.keysym.scancode;
			}

			switch (activeKey) {
			case SDL_SCANCODE_W:
			case SDL_SCANCODE_UP: {
				MoveSprite(rigidBody, sprite, transform, 0, -1, 0, Direction::Up);
				break;
			}
			case SDL_SCANCODE_D:
			case SDL_SCANCODE_RIGHT: {
				MoveSprite(rigidBody, sprite, transform, 1, 0, 1, Direction::Right);
				break;
			}
			case SDL_SCANCODE_S:
			case SDL_SCANCODE_DOWN: {
				MoveSprite(rigidBody, sprite, transform, 0, 1, 2, Direction::Down);
				break;
			}
			case SDL_SCANCODE_A:
			case SDL_SCANCODE_LEFT: {
				MoveSprite(rigidBody, sprite, transform, -1, 0, 3, Direction::Left);
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
		else {
			activeKey = SDL_SCANCODE_UNKNOWN;
		}
	}

	void MenuControl(const KeyPressedEvent& e) {
		// todo rigolo  - at this point nothing has a menu component
		auto view = reg.view<MenuComponent>();
		for (auto entity : view) {
			auto& menu = view.get<MenuComponent>(entity);

			if (!menu.isActive) return;

			if (e.event.type == SDL_KEYDOWN) {
				switch (e.event.key.keysym.scancode) {
				case SDL_SCANCODE_UP:
					textSystem.ClearText(); // TODO RIGOLO - this is still awful - need to get emplace_or_update working in renderTextSystem.h
					menu.currentIndex = (menu.currentIndex - 1 + menu.options.size()) % menu.options.size();
					std::cout << menu.currentIndex << std::endl;
					dispatcher.enqueue<MenuNavigateEvent>();
					break;
				case SDL_SCANCODE_DOWN:
					textSystem.ClearText();
					menu.currentIndex = (menu.currentIndex + 1) % menu.options.size();
					std::cout << menu.currentIndex << std::endl;
					dispatcher.enqueue<MenuNavigateEvent>();
					break;
				case SDL_SCANCODE_X: 
					// confirm
					spdlog::info("Selected: {}", menu.options[menu.currentIndex]);
					if (menu.options[menu.currentIndex] == "Talk") {
						auto view = reg.view<PlayerComponent, TransformComponent, RigidBodyComponent>();
						const auto entity = view.front();
						const auto& player = view.get<PlayerComponent>(entity);
						auto& rigidBody = view.get<RigidBodyComponent>(entity);
						const auto& transform = view.get<TransformComponent>(entity);

						dispatcher.enqueue<TalkEvent>(transform.position, rigidBody.direction);
					}
					break;
				case SDL_SCANCODE_Z: 
					// cancel
					menu.isActive = false;
					dispatcher.enqueue<MenuCloseEvent>();
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

	void onKeyUp(const KeyUpEvent& e) {
		auto view = reg.view<PlayerComponent, RigidBodyComponent, SpriteComponent, TransformComponent>();
		entt::entity player = view.front();

		RigidBodyComponent& rigidBody = view.get<RigidBodyComponent>(player);


		if (e.event.type != SDL_KEYDOWN) {
			activeKey = SDL_SCANCODE_UNKNOWN;
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