#pragma once

#include <entt.hpp>
#include "../src/assetstore/AssetStore.h"
#include <SDL.h>
#include <memory>
#include <sol/sol.hpp>

class MapLoader {
public:
	MapLoader();
	~MapLoader();

	void LoadMap(sol::state& lua, entt::registry& registry, const std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, std::string mapName);
};