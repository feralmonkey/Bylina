#pragma once

#include <SDL.h>
#include "entt.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <unordered_map>
#include "../libs/nlohmann/json.hpp"
#include "../components/TextComponent.h"


inline void RenderTextSystem(entt::registry& registry, SDL_Renderer* renderer, SDL_Rect camera, std::unique_ptr<AssetStore>& assetStore) {
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

		// simple attributes - will change what we need to
		int x = textLabel.position.x;
		int y = textLabel.position.y;
		int tileScale = 1;
		int tileSize = 8;

		// get the first character and convert hex character to integer

		// TODO RIGOLO draw the top line of the text box
		// draw top left corner
		entt::entity topLeft = registry.create();
		int srcRectX = 0;
		int srcRectY = 0;
		registry.emplace<TransformComponent>(topLeft, glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
		registry.emplace<SpriteComponent>(topLeft, "character-tiles", tileSize, tileSize, 10, false, srcRectX, srcRectY);   // srcRectX and srcRectY = 0 , 0 for top left
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
