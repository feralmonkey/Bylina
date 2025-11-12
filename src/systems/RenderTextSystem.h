#pragma once

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
	SDL_Rect& camera;
	std::vector<InputState>& inputStack;
	std::unordered_map<char, std::pair<int, int>> charLookup;

	// menu pointer variables
	//int currentIndex = 0;
	std::vector<std::string> menuSelectTracker;

	// break the text into words
	static std::vector<std::string> splitWords(const std::string& text) {
	    std::vector<std::string> words;
	    std::string current;
	    for (char c : text) {
	        if (c == ' ') {
	            if (!current.empty()) {
	                words.push_back(current);
	                current.clear();
	            }
	            words.push_back(" "); // keep spaces
	        } else if (c == '/') {
	            // manual newline marker becomes its own "word"
	            if (!current.empty()) {
	                words.push_back(current);
	                current.clear();
	            }
	            words.push_back("\n");
	        } else {
	            current.push_back(c);
	        }
	    }
	    if (!current.empty()) {
	    words.push_back(current);
	    }
	    return words;
	}

	// turn text into lines that fit the box width (in characters)
	static std::vector<std::string> wrapToLines(const std::string& text, const int boxInnerWidth) {
	    // boxInnerWidth = textLabel.width - 2 (borders)
	    const auto words = splitWords(text);
	    std::vector<std::string> lines;
	    std::string line;

	    for (auto& w : words) {
	        if (w == "\n") {
	            // force line break
	            lines.push_back(line);
	            line.clear();
	            continue;
	        }

	        // try adding this word
	        std::string candidate = line;
	        if (!candidate.empty()) candidate += w;
	        else candidate = w;

	        if (static_cast<int>(candidate.size()) > boxInnerWidth) {
	            // if the current line is full, push and start a new one
	            if (!line.empty()) {
	                lines.push_back(line);
	            }
	            // if the word itself is longer than the line, just push it as is
	            if (static_cast<int>(w.size()) > boxInnerWidth) {
	                lines.push_back(w.substr(0, boxInnerWidth));
	                line.clear();
	            } else {
	                line = w;
	            }
	        } else {
	            line = candidate;
	        }
	    }

	    if (!line.empty()) {
	        lines.push_back(line);
	    }

	    return lines;
	}

	// split lines into pages based on box height
	static std::vector<std::vector<std::string>> paginate(
	    const std::vector<std::string>& lines,
	    const int boxInnerHeight
	) {
	    // boxInnerHeight = textLabel.height - 2 (top/bottom border)
	    std::vector<std::vector<std::string>> pages;
	    for (size_t i = 0; i < lines.size();) {
	        std::vector<std::string> page;
	        for (int r = 0; r <= boxInnerHeight && i < lines.size(); ++r, ++i) {
	            page.push_back(lines[i]);
	        }
	        pages.push_back(std::move(page));
	    }
	    return pages;
	}

	void RenderTextBox() {
	    auto view = registry.view<TextComponent>();

	    for (const auto entity : view) {
	        const auto& textLabel = view.get<TextComponent>(entity);

	        int x = camera.x + textLabel.xOffset;
	        int y = camera.y + textLabel.yOffset;

	        // 1. draw frame (same as before)
	        // top-left
	        DrawChar(registry, 0, 0, x, y);
	        x += tileSize;

	        // top border
	        for (int i = 1; i < textLabel.width; i++) {
	            DrawChar(registry, 32, 0, x, y);
	            x += tileSize;
	        }
	        // top-right
	        DrawChar(registry, 8, 0, x, y);

	        // reset
	        x = camera.x + textLabel.xOffset;
	        y += tileSize;

	        // figure out what page to show
	        const auto& pages = textLabel.pages;
	        const int pageIndex = textLabel.currentPage;
	        const bool hasMore = (pageIndex + 1 < static_cast<int>(pages.size()));

	        const auto& lines = pages.empty() ? std::vector<std::string>{} : pages[pageIndex];

	        const int innerHeight = textLabel.height - 2;
	        const int innerWidth  = textLabel.width  - 2;

	        // 2. draw text rows
	        for (int row = 0; row < innerHeight; ++row) {
	            // left border
	            DrawChar(registry, 48, 0, x, y);
	            x += tileSize;

	            // the line we want to draw (if any)
	            std::string line = (row < static_cast<int>(lines.size())) ? lines[row] : "";

	            // draw characters
	            for (int col = 0; col <= innerWidth; ++col) {
	                if (col < static_cast<int>(line.size())) {
	                    char character = line[col];
	                    auto [tileX, tileY] = charLookup[character];
	                    DrawChar(registry, tileX, tileY, x, y);
	                } else {
	                    // blank
	                    DrawChar(registry, 88, 24, x, y);
	                }
	                x += tileSize;
	            }

	            // right border
	            DrawChar(registry, 56, 0, x, y);
	            y += tileSize;
	            x = camera.x + textLabel.xOffset;
	        }

	        // 3. draw bottom border
	        // bottom-left
	        DrawChar(registry, 16, 0, x, y);
	        x += tileSize;
	        // bottom border
	        for (int i = 1; i < textLabel.width; i++) {
	            DrawChar(registry, 40, 0, x, y);
	            x += tileSize;
	        }
	        // bottom-right
	        DrawChar(registry, 24, 0, x, y);

	        // 4. draw "more" indicator if there are more pages
	        if (hasMore) {
	            // put it above bottom-right, or bottom-left; your choice
	            // here's bottom-right-1 inside the box
	            int moreX = camera.x + textLabel.xOffset + (textLabel.width - 2) * tileSize;
	            int moreY = camera.y + textLabel.yOffset + (textLabel.height - 2) * tileSize;
	            // assume '>' is in your charLookup
	            auto [mx, my] = charLookup['>'];
	            DrawChar(registry, mx, my, moreX, moreY);
	        }
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

	void DrawChar(entt::registry& registry, const int srcx, const int srcy, const int dstx, const int dsty) {
		const entt::entity textWindow = registry.create();
		registry.emplace<SpriteComponent>(textWindow, "character-tiles", tileSize, tileSize, 10, false, srcx, srcy);
		registry.emplace<TransformComponent>(textWindow, glm::vec2(dstx * (tileScale), dsty * (tileScale)), glm::vec2(tileScale, tileScale), 0.0);
		registry.emplace<SpriteTag>(textWindow);
	}

public:
	RenderTextSystem(entt::registry& registry, entt::dispatcher& dispatcher, SDL_Rect& camera, std::vector<InputState>& inputStack, int tileScale = 1, int tileSize = 8) :
		registry(registry),
		dispatcher(dispatcher),
		camera(camera),
		inputStack(inputStack)
	{
		this->tileScale = tileScale;
		this->tileSize = tileSize;

		std::ifstream charMap("./assets/tilemaps/charmap.json");
		if (!charMap.is_open()) {
			spdlog::error("could not open charmap.json!");
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
			.connect<&RenderTextSystem::RenderAllMenus>(*this);   // todo rigolo - this function needs to be updated
	}

	// possibly duplicate efforts with Clear Text. It works right now but maybe can clean up
	inline void ClearTextBox() const {

		// if player hits cancel button, and we are already at the bottom of the stack
		if (inputStack.back() == InputState::PlayerControl) {
			return;
		}

		// This works by clearing the screen. The images get written again immediately next frame
		auto view = registry.view<SpriteTag>();
		for (const auto entity : view) {
			registry.destroy(entity);
		}

		// take control away from the text box
		inputStack.pop_back();
	}

	// more of a clear screen...
	void ClearText() const {
		auto view = registry.view<SpriteTag>();
		for (const auto entity : view) {
			registry.destroy(entity);
		}
	}

	void RenderAllMenus() {
		auto view = registry.view<MenuComponent>();
		for (const auto entity : view) {
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

		// inner space (no borders)
		const int innerWidth  = width  - 2;
		const int innerHeight = height - 2;

		// wrap + paginate
		const auto lines = wrapToLines(message, innerWidth);
		auto pages = paginate(lines, innerHeight);

		auto& tc = registry.emplace_or_replace<TextComponent>(textBoxEntity, message, width, height, xOffset, yOffset);
		tc.pages = std::move(pages);
		tc.currentPage = 0;

		RenderTextBox();
	}
};