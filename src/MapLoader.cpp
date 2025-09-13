#include "./MapLoader.h"
#include "./Game.h"
#include <fstream>
#include <sol/sol.hpp>
#include <spdlog/spdlog.h> 
#include "../src/components/TransformComponent.h"
#include "../src/components/RigidBodyComponent.h"
#include "../src/components/SpriteComponent.h"
#include "../src/components/AnimationComponent.h"
#include "../src/components/BoxColliderComponent.h"
#include "../src/components/KeyboardControlComponent.h"
#include "../src/components/CameraFollowComponent.h"
#include "../src/components/HealthComponent.h"
#include "../src/components/ScriptComponent.h"

MapLoader::MapLoader() {
	spdlog::info("MapLoader constructer called");
}

MapLoader::~MapLoader() {
	spdlog::info("MapLoader destructor called");
}

void MapLoader::LoadMap(sol::state& lua, entt::registry& registry, const std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, std::string mapName) {
	// load and test the script
	sol::load_result script = lua.load_file("./assets/scripts/" + mapName + ".lua");
	if (!script.valid()) {
		sol::error err = script;
		std::string errorMessage = err.what();
		spdlog::error("This script is horse shit! Fix it! : " + errorMessage);
		return;
	}
	// TODO RIGOLO Testing Block | works
	spdlog::info("so far so good - i.e. a valid script");
	return; 

	// executes the script using the lua state
	lua.script_file("./assets/scripts/" + mapName + ".lua");
	spdlog::info("We just opened map " + mapName + ".lua");

	// read the big table that has data for current map
	sol::table map = lua["Map"];


	////////////////////////////////////////////////////////////////
	// Read map assets
	////////////////////////////////////////////////////////////////
#pragma region LOAD MAP ASSETS
	sol::table assets = map["assets"];

	// loop all entries of asset table
	int i = 0;
	while (true) {
		sol::optional<sol::table> hasAsset = assets[i];
		if (hasAsset == sol::nullopt) {
			// if table we read at given position is null, break out of loop
			break;
		}

		sol::table asset = assets[i];
		std::string assetId = asset["id"];
		std::string assetType = asset["type"];

		if (assetType == "texture") {
			assetStore->AddTexture(renderer, assetId, asset["file"]);
			spdlog::info("New texture added to Asset Store: Id:" + assetId);
		}
		if (assetType == "font") {
			assetStore->AddFont(assetId, asset["file"], asset["font_size"]);
			spdlog::info("New font added to Asset Store: Id:" + assetId);
		}
		i++;
	}
#pragma endregion

	////////////////////////////////////////////////////////////////
	// Read map tilemap
	////////////////////////////////////////////////////////////////
#pragma region LOAD MAP TILEMAP
	sol::table tilemap = map["tilemap"];

	// read map file 
	int tileSize = tilemap["tile_size"];
	double tileScale = tilemap["scale"];
	int numMapCols = tilemap["num_cols"];
	int numMapRows = tilemap["num_rows"];
	std::string map_file = tilemap["map_file"];
	std::string asset_id = tilemap["texture_asset_id"];

	std::fstream mapFile;
	mapFile.open(map_file);

	for (int y = 0; y < numMapRows; y++) {
		for (int x = 0; x < numMapCols; x++) {
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			entt::entity tile = registry.create();
			// tile.Group("tiles");  // todo rigolo - add group and tagging support
			registry.emplace<TransformComponent>(tile, glm::vec2(x * (tileSize * tileScale), y * (tileSize * tileScale)), glm::vec2(tileScale, tileScale), 0.0);
			registry.emplace<SpriteComponent>(tile, asset_id, tileSize, tileSize, 0, false, srcRectX, srcRectY);
		}
	}
	mapFile.close();
	Game::mapWidth = numMapCols * tileSize * tileScale;
	Game::mapHeight = numMapRows * tileSize * tileScale;
#pragma endregion

#pragma region LOAD MAP ENTITIES
	////////////////////////////////////////////////////////////////
	// Read map entities
	////////////////////////////////////////////////////////////////
	sol::table entities = map["entities"];
	i = 0;
	while (true) {
		sol::optional<sol::table> hasEntity = entities[i];
		if (hasEntity == sol::nullopt) {
			break;
		}

		sol::table entity = entities[i];

		entt::entity newEntity = registry.create();

		// Tag
		sol::optional<std::string> tag = entity["tag"];
		if (tag != sol::nullopt) {
			//newEntity.Tag(entity["tag"]); // todo rigolo - implement tagging and grouping
		}

		// Group
		sol::optional<std::string> group = entity["group"];
		if (group != sol::nullopt) {
			//newEntity.Group(entity["group"]); // todo rigolo - implement tagging and grouping
		}

		// Components
		sol::optional<sol::table> hasComponents = entity["components"];
		if (hasComponents != sol::nullopt) {
			// Transform
			sol::optional<sol::table> transform = entity["components"]["transform"];
			if (transform != sol::nullopt) {
				registry.emplace<TransformComponent>(
					newEntity,
					glm::vec2(
						entity["components"]["transform"]["position"]["x"],
						entity["components"]["transform"]["position"]["y"]
					),
					glm::vec2(
						entity["components"]["transform"]["scale"]["x"].get_or(1.0),
						entity["components"]["transform"]["scale"]["y"].get_or(1.0)
					),
					entity["components"]["transform"]["rotation"].get_or(0.0)
				);
			}

			// RigidBody
			sol::optional<sol::table> rigidbody = entity["components"]["rigidbody"];
			if (rigidbody != sol::nullopt) {
				registry.emplace<RigidBodyComponent>(
					newEntity,
					glm::vec2(
						entity["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
						entity["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)
					)
				);
			}

			// Sprite
			sol::optional<sol::table> sprite = entity["components"]["sprite"];
			if (sprite != sol::nullopt) {
				newEntity,
				registry.emplace<SpriteComponent>(
					entity["components"]["sprite"]["texture_asset_id"],
					entity["components"]["sprite"]["width"],
					entity["components"]["sprite"]["height"],
					entity["components"]["sprite"]["z_index"].get_or(1),
					entity["components"]["sprite"]["fixed"].get_or(false),
					entity["components"]["sprite"]["src_rect_x"].get_or(0),
					entity["components"]["sprite"]["src_rect_y"].get_or(0)
				);
			}

			// Animation
			sol::optional<sol::table> animation = entity["components"]["animation"];
			if (animation != sol::nullopt) {
				registry.emplace<AnimationComponent>(
					newEntity,
					entity["components"]["animation"]["num_frames"].get_or(1),
					entity["components"]["animation"]["speed_rate"].get_or(1)
				);
			}

			// Box Collider
			sol::optional<sol::table> boxcollider = entity["components"]["boxcollider"];
			if (boxcollider != sol::nullopt) {
				registry.emplace<BoxColliderComponent>(
					newEntity,
					entity["components"]["boxcollider"]["width"],
					entity["components"]["boxcollider"]["height"],
					glm::vec2(
						entity["components"]["boxcollider"]["offset"]["x"].get_or(0),
						entity["components"]["boxcollider"]["offset"]["y"].get_or(0)
					)
				);
			}

			// Health
			sol::optional<sol::table> health = entity["components"]["health"];
			if (health != sol::nullopt) {
				registry.emplace<HealthComponent>(
					newEntity,
					static_cast<int>(entity["components"]["health"]["health_percentage"].get_or(100))
				);
			}

			// CAMERA FOLLOW
			sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
			if (cameraFollow != sol::nullopt) {
				registry.emplace<CameraFollowComponent>(
					newEntity,
					static_cast<int>(entity["components"]["camera_follow"]["follow"].get_or(false))
				);
			}

			// Keyboard Controller
			sol::optional<sol::table> keyboard = entity["components"]["keyboard_controller"];
			if (keyboard != sol::nullopt) {
				registry.emplace<KeyboardControlComponent>(
					newEntity,
					glm::vec2(
						entity["components"]["keyboard_controller"]["up_velocity"]["x"],
						entity["components"]["keyboard_controller"]["up_velocity"]["y"]
					),
					glm::vec2(
						entity["components"]["keyboard_controller"]["right_velocity"]["x"],
						entity["components"]["keyboard_controller"]["right_velocity"]["y"]
					),
					glm::vec2(
						entity["components"]["keyboard_controller"]["down_velocity"]["x"],
						entity["components"]["keyboard_controller"]["down_velocity"]["y"]
					),
					glm::vec2(
						entity["components"]["keyboard_controller"]["left_velocity"]["x"],
						entity["components"]["keyboard_controller"]["left_velocity"]["y"]
					)
				);
			}

			// Script Component
			sol::optional<sol::table> script = entity["components"]["on_update_script"];
			if (script != sol::nullopt) {
				sol::function func = entity["components"]["on_update_script"][0];
				registry.emplace<ScriptComponent>(newEntity, func);
			}
		}
		i++;
	}
}
#pragma endregion
