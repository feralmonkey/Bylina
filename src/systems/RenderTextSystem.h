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

		// testing
		//std::cout << "Value for 'A': " << unorderedMap["A"] << std::endl;
		//std::cout << "Value for 'b': " << unorderedMap["b"] << std::endl;

		// get the first character and convert hex character to integer
		int x = textLabel.position.x;
		int y = textLabel.position.y;

		// draw the top line of the text box
		// draw top left corner
		DrawChar(registry, 0, 0, x, y);
		x++;

		//// draw top border
		for (int i = 1; i < textLabel.width; i++) {
			DrawChar(registry, 32, 0, x, y);
			x++;
		}

		//// draw top right corner
		DrawChar(registry, 8, 0, x, y);
		
		//// reset values for next line
		x = textLabel.position.x;
		y++;


		// draw lines
		for (int i = 1; i < textLabel.height - 1; i++) {
			// left border
			std::cout << "width: " << textLabel.width - 2 << std::endl;
			DrawChar(registry, 48, 0, x, y);
			x++;
			for (int j = 1; j < textLabel.width; j++) {
				// TODO RIGOLO - NEED TO PARSE TEXT HERE
				DrawChar(registry, 88, 24, x, y);
				x++;
			}
			DrawChar(registry, 56, 0, x, y);
			x = textLabel.position.x;
			y++;
		}

		// draw bottom left corner
		DrawChar(registry, 16, 0, x, y);
		x++;

		//// draw bottom border
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

// iterates through the text TODO RIGOLO - there's a max length so this won't work
//for (char c : textLabel.text) {
//	std::cout << unorderedMap[std::string(1,c)] << ",";
//}
//std::cout << std::endl;

// TODO Rigolo - draw the bottom of the text box

// TODO Rigolo - RENDER



/*   ALL BELOW IS REFERENCE*/

//for (int y = 0; y < mapNumRows; y++) {
//	for (int x = 0; x < mapNumCols; x++) {
//		char ch;

//		// get the first character and convert hex character to integer
//		mapFile.get(ch);
//		int valueY = std::stoi(std::string(1, ch), nullptr, 16);
//		int srcRectY = valueY * tileSize;

//		// get second character and convert
//		mapFile.get(ch);
//		int valueX = std::stoi(std::string(1, ch), nullptr, 16);
//		int srcRectX = valueX * tileSize;

//		// ignore commas
//		mapFile.ignore();

//		entt::entity tile = registry.create();
//		/*tile.Group("tiles");*/
//		registry.emplace<TransformComponent>(tile, glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
//		registry.emplace<SpriteComponent>(tile, "outdoor-tiles", tileSize, tileSize, 0, false, srcRectX, srcRectY);
//	}
//}

// render the virtual .map similar to how the background is rendered
// somehow give modality to the text box
// need to handle multiple screens

//	

//	SDL_Surface* surface = TTF_RenderText_Blended(
//		assetStore->GetFont(textLabel.assetId),
//		textLabel.text.c_str(),
//		textLabel.color
//	);
//	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
//	SDL_FreeSurface(surface);

//	int labelWidth = 0;
//	int labelHeight = 0;

//	SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

//	SDL_Rect dstRect = {
//		static_cast<int>(textLabel.position.x - (textLabel.isFixed ? 0 : camera.x)),
//		static_cast<int>(textLabel.position.y - (textLabel.isFixed ? 0 : camera.y)),
//		labelWidth,
//		labelHeight
//	};

//	SDL_RenderCopy(renderer, texture, NULL, &dstRect);
//	SDL_DestroyTexture(texture);
