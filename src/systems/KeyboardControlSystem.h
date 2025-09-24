#pragma once
#include <iostream>
#include <SDL.h>
#include "entt.hpp"
#include <spdlog/spdlog.h>
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
#include "../events/MenuOpenEvent.h"
#include "../events/MenuCloseEvent.h"
#include "../events/MenuNavigateEvent.h"
#include "../enums/InputState.h"


class KeyboardControlSystem : public ISystem {

private:
	entt::registry& reg;
	entt::dispatcher& dispatcher;
	std::vector<InputState>& inputStack;
	RenderTextSystem& textSystem;
	InputState currentInput;

	void PlayerControl(const KeyPressedEvent& e) {
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
				rigidBody.velocity.y =  0;
				rigidBody.lastMoveDir = { -1.0f, 0.0f };
				sprite.srcRect.y = sprite.height * 3;
				break;
			}
			case SDL_SCANCODE_D:
			case SDL_SCANCODE_RIGHT: {
				rigidBody.velocity.x = +1;
				rigidBody.velocity.y = 0;
				rigidBody.lastMoveDir = { +1.0f, 0.0f };
				sprite.srcRect.y = sprite.height * 1;
				break;
			}
			case SDL_SCANCODE_W:
			case SDL_SCANCODE_UP: {
				rigidBody.velocity.y = -1;
				rigidBody.velocity.y = -1;
				rigidBody.lastMoveDir = { 0.0f, -1.0f };
				sprite.srcRect.y = sprite.height * 0;
				break;
			}
			case SDL_SCANCODE_S:
			case SDL_SCANCODE_DOWN: {
				rigidBody.velocity.y = +1;
				rigidBody.velocity.y = +1;
				rigidBody.lastMoveDir = { 0.0f, +1.0f };
				sprite.srcRect.y = sprite.height * 2;
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
		/*else if (e.event.type == SDL_KEYUP) {*/
		else {
			activeKey = SDL_SCANCODE_UNKNOWN;
			rigidBody.velocity.x = 0;
			rigidBody.velocity.y = 0;
		}
	}

	void ClearOld() {
		auto text_view = reg.view<TextComponent>();
		for (auto entity : text_view) {
			reg.destroy(entity);
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
					// TODO: trigger action depending on option
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
			dispatcher.enqueue<MenuOpenEvent>();
			/*std::cout << "MenuControl" << std::endl;*/
			MenuControl(e);
			return;
		}
		std::cout << "NoControl" << std::endl;
	}
};