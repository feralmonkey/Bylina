#pragma once

# include <glm/glm.hpp>
# include <string>
# include <SDL.h>

struct SpriteComponent {
	public:
		std::string assetId;
		int width;
		int height;
		int zIndex;
		SDL_RendererFlip flip;
		SDL_Rect srcRect;
		bool isFixed;

		//SpriteComponent(std::string assetId = "", int width = 0, int height = 0, int zIndex = 0, int srcRectX = 0, int srcRectY = 0, bool isFixed = false) {
		SpriteComponent(std::string assetId = "", int width = 0, int height = 0, int zIndex = 0, bool isFixed = false, int srcRectX = 0, int srcRectY = 0) {
			this->assetId = assetId;
			this->width = width;
			this->height = height;
			this->zIndex = zIndex;
			this->flip = SDL_FLIP_NONE;
			this->isFixed = isFixed;
			this->srcRect = { srcRectX, srcRectY, width, height };
		};

		bool compareZindex(const SpriteComponent& a, const SpriteComponent& b) {
			return a.zIndex < b.zIndex;
		}
};
