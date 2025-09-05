#pragma once

#include <SDL.h>
#include "entt.hpp"
#include <spdlog/spdlog.h>
#include "../components/BoxColliderComponent.h"
#include "../components/TransformComponent.h"
#include "../components/TextLabelComponent.h"

inline void RenderTextSystem(entt::registry& reg, SDL_Renderer* renderer, SDL_Rect camera, std::unique_ptr<AssetStore>& assetStore) {
	auto view = reg.view<TextLabelComponent>(); 

	for (auto entity : view) {
		const auto& textLabel = view.get<TextLabelComponent>(entity);

		SDL_Surface* surface = TTF_RenderText_Blended(
			assetStore->GetFont(textLabel.assetId),
			textLabel.text.c_str(),
			textLabel.color
		);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		int labelWidth = 0;
		int labelHeight = 0;

		SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

		SDL_Rect dstRect = {
			static_cast<int>(textLabel.position.x - (textLabel.isFixed ? 0 : camera.x)),
			static_cast<int>(textLabel.position.y - (textLabel.isFixed ? 0 : camera.y)),
			labelWidth,
			labelHeight
		};

		SDL_RenderCopy(renderer, texture, NULL, &dstRect);
		SDL_DestroyTexture(texture);

	}
}