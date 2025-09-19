#pragma once

#include <SDL.h>
#include "entt.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <unordered_map>
#include <vector>
#include "../libs/nlohmann/json.hpp"
#include "../components/TextComponent.h"

class RenderTextSystem {

private:
	int tileScale = 1;
	int tileSize = 8;
	entt::registry& registry;
	SDL_Renderer* renderer;
	SDL_Rect& camera;
	std::unique_ptr<AssetStore>& assetStore;

	// menu pointer variables
	int index = 0;
	std::vector<std::string> menuSelectTracker = {
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" "
	};

public:
	RenderTextSystem(entt::registry& registry, SDL_Renderer* renderer, SDL_Rect& camera, std::unique_ptr<AssetStore>& assetStore, int tileScale = 1, int tileSize = 8) :
	registry(registry),
	renderer(renderer),
	camera(camera),
	assetStore(assetStore)
	{
		this->tileScale = tileScale;
		this->tileSize = tileSize;
	}

	void DrawChar(entt::registry& registry, int srcx, int srcy, int dstx, int dsty) {
		//std::cout << "dstx, dsty: " << dstx << " , " << dsty << std::endl;
		entt::entity textWindow = registry.create();
		registry.emplace<SpriteComponent>(textWindow, "character-tiles", tileSize, tileSize, 10, false, srcx, srcy);
		registry.emplace<TransformComponent>(textWindow, glm::vec2(dstx * (tileScale), dsty * (tileScale)), glm::vec2(tileScale, tileScale), 0.0);
		registry.emplace<SpriteTag>(textWindow);
	}

	inline void ClearTextBox() {
		auto view = registry.view<SpriteTag>();
		for (auto entity : view) {
			registry.destroy(entity);
		}
	}

	void RenderTextBox() {
		auto view = registry.view<TextComponent>();

		// TODO RIGOLO - may not actually need a loop here if there will only be one entity at a time with a TextComponent. 
		for (auto entity : view) {
			const auto textLabel = view.get<TextComponent>(entity);
			
			//std::cout << "camera x,y: " << camera.x << "," << camera.y << std::endl;

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
						std::string charValue = unorderedMap[std::string(1, character)];

						int valueY = std::stoi(std::string(1, charValue[0]), nullptr, 16);
						int srcRectY = valueY * tileSize;

						// get second character and convert
						int valueX = std::stoi(std::string(1, charValue[1]), nullptr, 16);
						int srcRectX = valueX * tileSize;

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

			charMap.close();
		}
	}

	void TownMenu() {
		entt::entity textBox = registry.create();
		menuSelectTracker[index] = ">";
		std::string menuText = std::string(" /") + 
			menuSelectTracker[0] + "Talk / /" + 
			menuSelectTracker[1] + "Cast/ /" + 
			menuSelectTracker[2] + "Use/ /" + 
			menuSelectTracker[3] + "Search/ /" +
			menuSelectTracker[4] + "Status/ /" +
			menuSelectTracker[5] + "Equip/ /" +
			menuSelectTracker[6] + "Order";
		registry.emplace<TextComponent>(textBox, menuText, 9, 16, 8, 16, true);
		RenderTextBox();
	}

	void TextBox(std::string message) {
		entt::entity textBox = registry.create();
		registry.emplace<TextComponent>(textBox, message, 18, 8, 40, 176, true);
		RenderTextBox();
	}
};