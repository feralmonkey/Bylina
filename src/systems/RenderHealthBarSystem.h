#pragma once

#include <spdlog/spdlog.h> 
#include <SDL.h>
#include "../assetstore/AssetStore.h"
#include "../ecs/ecs.h"
#include "../components/HealthComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/TransformComponent.h"

class RenderHealthBarSystem : public System {
public:
	RenderHealthBarSystem() {
		RequireComponent<HealthComponent>();
		RequireComponent<SpriteComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& assetStore, const SDL_Rect camera) {
		for (auto entity : GetSystemEntities()) {
			const auto health = entity.GetComponent<HealthComponent>();
			const auto sprite = entity.GetComponent<SpriteComponent>();
			const auto transform = entity.GetComponent<TransformComponent>();

			// health bar color is green by default
			SDL_Color healthBarColor = { 0, 255, 0 };

			if (health.healthPercentage < 40) {
				// 0 - 40 = red
				healthBarColor = { 255, 0, 0 };
			}
			if (health.healthPercentage >= 40 && health.healthPercentage < 80) {
				// 40 - 80 = yellow
				healthBarColor = { 255, 255, 0 };
			}

			// position the health bar indicator at the top right part of the entity sprite
			int healthBarWidth = 15;
			int healthBarHeight = 3;
			double healthBarPosX = (transform.position.x + (sprite.width * transform.scale.x)) - camera.x;
			double healthBarPosY = transform.position.y - camera.y;

			SDL_Rect healthBarRectangle = {
				static_cast<int>(healthBarPosX),
				static_cast<int>(healthBarPosY),
				static_cast<int>(healthBarWidth * (health.healthPercentage / 100.0)),
				static_cast<int>(healthBarHeight)
			};

			SDL_SetRenderDrawColor(renderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, 255);
			SDL_RenderFillRect(renderer, &healthBarRectangle);

			// render the health percentage text label indicator
			std::string healthText = std::to_string(health.healthPercentage);
			SDL_Surface* surface = TTF_RenderText_Blended(assetStore->GetFont("pico8-font-10"), healthText.c_str(), healthBarColor);
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);

			int labelWidth = 0;
			int labelHeight = 0;
			SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);
			SDL_Rect healthBarTextRectangle = {
				static_cast<int>(healthBarPosX),
				static_cast<int>(healthBarPosY) + 5,
				labelWidth,
				labelHeight
			};

			SDL_RenderCopy(renderer, texture, NULL, &healthBarTextRectangle);
			
			SDL_DestroyTexture(texture);
		}
	}
};