#pragma once
#include "GameConfig.h"
#include "assetstore/AssetStore.h"
#include <entt.hpp>
#include <memory>
#include <SDL.h>
#include <sol/sol.hpp>

class MapLoader {
public:
	MapLoader();
	~MapLoader();

	void LoadMap(sol::state& lua, entt::registry& registry, const std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, std::string mapName, GameConfig& gameConfig);
};