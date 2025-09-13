#pragma once

#include <SDL.h>
#include "entt.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <unordered_map>
#include "../libs/nlohmann/json.hpp"
#include "../components/TextComponent.h"

inline void DrawChar(entt::registry& registry, int srcx, int srcy, int dstx, int dsty) {
	int tileScale = 1;
	int tileSize = 8;

	entt::entity textWindow = registry.create();
	registry.emplace<TransformComponent>(textWindow, glm::vec2(dstx * (tileScale * tileSize), dsty * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
	registry.emplace<SpriteComponent>(textWindow, "character-tiles", tileSize, tileSize, 10, false, srcx, srcy); 
}

inline void RenderTextBox(entt::registry& registry, SDL_Renderer* renderer, SDL_Rect camera, std::unique_ptr<AssetStore>& assetStore) {
	auto view = registry.view<TextComponent>();

	// TODO RIGOLO - may not actually need a loop here if there will only be one entity at a time with a TextComponent. 
	for (auto entity : view) {
		const auto& textLabel = view.get<TextComponent>(entity);

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
			unorderedMap[it.key()] = it.value();
		}

		// get the first character and convert hex character to integer
		int x = textLabel.position.x;
		int y = textLabel.position.y;

		// draw top left corner
		DrawChar(registry, 0, 0, x, y);
		x++;

		// draw top border
		for (int i = 1; i < textLabel.width; i++) {
			DrawChar(registry, 32, 0, x, y);
			x++;
		}

		// draw top right corner
		DrawChar(registry, 8, 0, x, y);
		
		// reset values for next line
		x = textLabel.position.x;
		y++;


		// draw text rows
		int textCounter = 0;
		bool pauseLine;
		for (int i = 1; i < textLabel.height - 1; i++) {
			// left border
			DrawChar(registry, 48, 0, x++, y);
			pauseLine = false;

			// draw characters
			for (int j = 1; j < textLabel.width; j++) {
				
				// if we still have additional characters in the text label then print the next character
				// otherwise just print a black square
				if (textCounter < textLabel.text.size()) {
					char character = textLabel.text[textCounter];

					if (character == '/'){
						if (j > 1) {
							pauseLine = true;
							DrawChar(registry, 88, 24, x++, y);
							textCounter++;
							continue;
						}
						else {
							continue;
						}
					}
					if (pauseLine) {
						DrawChar(registry, 88, 24, x++, y);
						continue;
					}
					
					textCounter++;
					/*std::cout << "x, y, txtctr, char:" << x << "," << y << "," << textCounter << "," << character << std::endl;*/
					std::string charValue = unorderedMap[std::string(1, character)];

					int valueY = std::stoi(std::string(1, charValue[0]), nullptr, 16);
					int srcRectY = valueY * 8;  // 8 is tileSize - if I classify this, make it a private class member

					// get second character and convert

					int valueX = std::stoi(std::string(1, charValue[1]), nullptr, 16);
					int srcRectX = valueX * 8;  // 8 is tileSize - if I classify this, make it a private class member

					DrawChar(registry, srcRectX, srcRectY, x++, y);
				}
				else {
					DrawChar(registry, 88, 24, x++, y);
				} 
			}

			// right border
			DrawChar(registry, 56, 0, x, y++);

			// reset values for next line
			x = textLabel.position.x;
			pauseLine = false;
		}

		// draw bottom left corner
		DrawChar(registry, 16, 0, x, y);
		x++;

		// draw bottom border
		for (int i = 1; i < textLabel.width; i++) {
			DrawChar(registry, 40, 0, x, y);
			x++;
		}

		//// draw bottom right corner
		DrawChar(registry, 24, 0, x, y);

		charMap.close();
	}
}

inline void ClearTextBox(entt::registry& registry) {
	auto view = registry.view<TextComponent>();
	for (auto entity : view) {
		if (auto* textBox = registry.try_get<TextComponent>(entity)) {
			registry.destroy(entity);
		}
	}
}