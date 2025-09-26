#pragma once

#include <iostream>
#include <SDL.h>
#include "entt.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <unordered_map>
#include <vector>
#include "../events/MenuOpenEvent.h"
#include "../events/MenuCloseEvent.h"
#include "../events/MenuNavigateEvent.h"
#include "../libs/nlohmann/json.hpp"
#include "../systems/ISystem.h"
#include "../components/SpriteComponent.h"
#include "../components/TextComponent.h"
#include "../components/MenuComponent.h"
#include "../components/TagComponents.h"
#include "../enums/InputState.h"

class RenderTextSystem {

private:
	int tileScale = 1;
	int tileSize = 8;
	entt::registry& registry;
	entt::dispatcher& dispatcher;
	SDL_Renderer* renderer;
	SDL_Rect& camera;
	std::unique_ptr<AssetStore>& assetStore;
	std::vector<InputState>& inputStack;
	std::unordered_map<char, std::pair<int, int>> charLookup;

	// menu pointer variables
	int currentIndex = 0;
	int itemCount;
	std::vector<std::string> menuSelectTracker;

	void RenderTextBox() {
		auto view = registry.view<TextComponent>();

		for (auto entity : view) {
			const auto textLabel = view.get<TextComponent>(entity);
			std::cout << "View Size: " << view.size() << std::endl;
			// get the first character and convert hex character to integer
			int x = camera.x + textLabel.xOffset;
			int y = camera.y + textLabel.yOffset;

			// draw top left corner
			DrawChar(registry, 0, 0, x, y);
			x += tileSize;

			// draw top border
			for (int i = 1; i < textLabel.width; i++) {
				DrawChar(registry, 32, 0, x, y);
				x += tileSize;
			}

			// draw top right corner
			DrawChar(registry, 8, 0, x, y);

			// reset values for next line
			x = camera.x + textLabel.xOffset;
			y += tileSize;


			// draw text rows
			int textCounter = 0;
			bool pauseLine;
			for (int i = 1; i < textLabel.height - 1; i++) {
				// left border
				DrawChar(registry, 48, 0, x, y);
				x += tileSize;
				pauseLine = false;

				// draw characters
				for (int j = 1; j < textLabel.width; j++) {

					// if we still have additional characters in the text label then print the next character
					// otherwise just print a black square
					if (textCounter < textLabel.text.size()) {
						char character = textLabel.text[textCounter];

						if (character == '/') {
							if (j > 1) {
								pauseLine = true;
								DrawChar(registry, 88, 24, x, y);
								x += tileSize;
								textCounter++;
								continue;
							}
							else {
								continue;
							}
						}
						if (pauseLine) {
							DrawChar(registry, 88, 24, x, y);
							x += tileSize;
							continue;
						}

						textCounter++;
						auto [tileX, tileY] = charLookup[character];
						int srcRectX = tileX;
						int srcRectY = tileY;

						DrawChar(registry, srcRectX, srcRectY, x, y);
						x += tileSize;
					}
					else {
						DrawChar(registry, 88, 24, x, y);
						x += tileSize;
					}
				}

				// right border
				DrawChar(registry, 56, 0, x, y);
				y += tileSize;

				// reset values for next line
				x = camera.x + textLabel.xOffset;
				pauseLine = false;
			}

			// draw bottom left corner
			DrawChar(registry, 16, 0, x, y);
			x += tileSize;

			// draw bottom border
			for (int i = 1; i < textLabel.width; i++) {
				DrawChar(registry, 40, 0, x, y);
				x += tileSize;
			}

			//// draw bottom right corner
			DrawChar(registry, 24, 0, x, y);
		}
	}

	void RenderMenu(const MenuComponent& menu) {
		if (!menu.isActive) return;

		std::string message = " /";

		for (size_t i = 0; i < menu.options.size(); i++) {
			if (i == menu.currentIndex) {
				message += ">" + menu.options[i] + "/ /";  // cursor
			}
			else {
				message += " " + menu.options[i] + "/ /";
			}
		}

		TextBox(message, 9, 16, 8, 16);
	}

	void DrawChar(entt::registry& registry, int srcx, int srcy, int dstx, int dsty) {
		//std::cout << "dstx, dsty: " << dstx << " , " << dsty << std::endl;
		entt::entity textWindow = registry.create();
		registry.emplace<SpriteComponent>(textWindow, "character-tiles", tileSize, tileSize, 10, false, srcx, srcy);
		registry.emplace<TransformComponent>(textWindow, glm::vec2(dstx * (tileScale), dsty * (tileScale)), glm::vec2(tileScale, tileScale), 0.0);
		registry.emplace<SpriteTag>(textWindow);
	}

public:
	RenderTextSystem(entt::registry& registry, entt::dispatcher& dispatcher, SDL_Renderer* renderer, SDL_Rect& camera, std::unique_ptr<AssetStore>& assetStore, std::vector<InputState>& inputStack, int tileScale = 1, int tileSize = 8) :
		registry(registry),
		dispatcher(dispatcher),
		renderer(renderer),
		camera(camera),
		assetStore(assetStore),
		inputStack(inputStack)
	{
		this->tileScale = tileScale;
		this->tileSize = tileSize;

		std::ifstream charMap("./assets/tilemaps/charMap.json");
		if (!charMap.is_open()) {
			spdlog::error("could not open charMap.json!");
			return;
		}

		// load the .json file into an unordered map
		std::unordered_map<std::string, std::string> unorderedMap;
		nlohmann::json jsonMap;
		charMap >> jsonMap;
		for (auto it = jsonMap.begin(); it != jsonMap.end(); ++it) {
			char key = it.key()[0]; // single character
			std::string val = it.value();
			int valueY = std::stoi(std::string(1, val[0]), nullptr, 16);
			int valueX = std::stoi(std::string(1, val[1]), nullptr, 16);
			charLookup[key] = { valueX * tileSize, valueY * tileSize };
		}

		// subscribe to all menu events
		dispatcher.sink<MenuOpenEvent>()
			.connect<&RenderTextSystem::RenderAllMenus>(*this);
		dispatcher.sink<MenuCloseEvent>()
			.connect<&RenderTextSystem::ClearTextBox>(*this);
		dispatcher.sink<MenuNavigateEvent>()
			.connect<&RenderTextSystem::RenderAllMenus>(*this);   // todo rigolo - this dfunction needs to be updated
	}

	inline void ClearTextBox() {

		// if player hits cancel button and we are already at the bottom of the stack
		if (inputStack.back() == InputState::PlayerControl) {
			return;
		}

		// TODO RIGOLO - wait - sprite tag? This works but now I've forgotten how!  maybe it works accidentally because we're not giving tiles and the character this tag?
		auto view = registry.view<SpriteTag>();
		for (auto entity : view) {
			registry.destroy(entity);
		}

		// take control away from the text box
		inputStack.pop_back();
	}

	void ClearText() {
		auto view = registry.view<SpriteTag>();
		for (auto entity : view) {
			registry.destroy(entity);
		}
	}

	void RenderAllMenus() {
		auto view = registry.view<MenuComponent>();
		for (auto entity : view) {
			const auto& menu = view.get<MenuComponent>(entity);
			if (menu.isActive) {
				RenderMenu(menu);
			}
		}
	}

	entt::entity textBoxEntity = entt::null;
	void TextBox(std::string message, int width = 18, int height = 8, int xOffset = 40, int yOffset = 176) {
		if (textBoxEntity == entt::null || !registry.valid(textBoxEntity)) {
			textBoxEntity = registry.create();
		}
		// TODO RIGOLO - this still doesn't work as it should : really look up emplace_or_replace
		registry.emplace_or_replace<TextComponent>(textBoxEntity, message, width, height, xOffset, yOffset);

		RenderTextBox();
	}

	void UpdateMenu(entt::entity entity, MenuComponent& menu) {
		std::string message = "//";
		for (size_t i = 0; i < menu.options.size(); i++) {
			if (i == menu.currentIndex) {
				message += "> " + menu.options[i] + "//";
			}
			else {
				message += "  " + menu.options[i] + "//";
			}
		}

		auto& text = registry.get<TextComponent>(entity);
		text.text = message;
	}
};