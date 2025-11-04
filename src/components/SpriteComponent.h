#pragma once

# include <string>
# include <SDL.h>

struct SpriteComponent {
	public:
		std::string assetId;
		int width;
		int height;
		int zIndex;
		int anchor_x;
		SDL_RendererFlip flip {};
		SDL_Rect srcRect {};
		bool isFixed;

		explicit SpriteComponent(const std::string& assetId = "", const int width = 0, const int height = 0, const int zIndex = 0, const bool isFixed = false, const int srcRectX = 0, const int srcRectY = 0) {
			this->assetId = assetId;
			this->width = width;
			this->height = height;
			this->zIndex = zIndex;
			this->anchor_x = srcRectX;
			this->flip = SDL_FLIP_NONE;
			this->isFixed = isFixed;
			this->srcRect = { srcRectX, srcRectY, width, height };
		};

		static bool compareZIndex(const SpriteComponent& a, const SpriteComponent& b) {
			return a.zIndex < b.zIndex;
		}
};
