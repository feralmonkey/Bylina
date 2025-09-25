#pragma once

#include <entt.hpp>
#include <spdlog/spdlog.h>
#include "../components/TagComponents.h"
#include "../components/BoxColliderComponent.h"
#include "../components/CollisionComponent.h"
#include "../components/TransformComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../events/CollisionEvent.h"
#include "../systems/ISystem.h"

class CollisionSystem : public ISystem {
private:
	entt::registry& registry;
	entt::dispatcher& dispatcher;
	int tileSize;
	int mapCols;
	int mapRows;
	int mapWidth;
	int mapHeight;


// TODO RIGOLO - Probably won't be using a dispatcher here
	// Remove dispatcher references

public:
	CollisionSystem(entt::registry& reg, entt::dispatcher& dis, int tileSize) 
		: registry(reg), dispatcher(dis), tileSize(tileSize)   { }

	// TODO RIGOLO - not all of this logic needs to be called every frame ; some can be in a map-refresh method
	void Update(int mapWidth, int mapHeight) {
		mapCols = mapWidth / tileSize;
		mapRows = mapHeight / tileSize;

		// 1D grid (each cell stores entities inside it)
		std::vector<std::vector<entt::entity>> grid(mapRows * mapCols);

		auto view = registry.view<BoxColliderComponent, TransformComponent>();


		// Insert entities into the grid
		for (auto entity : view) {
			const auto& transform = view.get<TransformComponent>(entity);
			const auto& collider = view.get<BoxColliderComponent>(entity);

			glm::vec2 pos = transform.position + collider.offset;
			glm::vec2 size = collider.size;

			int startCol = static_cast<int>(pos.x) / tileSize;
			int startRow = static_cast<int>(pos.y) / tileSize;
			int endCol = static_cast<int>(pos.x + size.x) / tileSize;
			int endRow = static_cast<int>(pos.y + size.y) / tileSize;

			startCol = std::max(0, startCol);
			startRow = std::max(0, startRow);
			endCol = std::min(mapCols - 1, endCol);
			endRow = std::min(mapRows - 1, endRow);

			for (int r = startRow; r <= endRow; ++r) {
				for (int c = startCol; c <= endCol; ++c) {
					int index = r * mapCols + c;
					grid[index].push_back(entity);
				}
			}
		}

		// Check collisions inside each tile
		for (int r = 0; r < mapRows; ++r) {
			for (int c = 0; c < mapCols; ++c) {
				int index = r * mapCols + c;
				auto& cellEntities = grid[index];

				for (size_t i = 0; i < cellEntities.size(); ++i) {
					auto entityA = cellEntities[i];
					const auto& transformA = view.get<TransformComponent>(entityA);
					const auto& colliderA = view.get<BoxColliderComponent>(entityA);
					glm::vec2 posA = transformA.position + colliderA.offset;
					glm::vec2 sizeA = colliderA.size;

					for (size_t j = i + 1; j < cellEntities.size(); ++j) {
						auto entityB = cellEntities[j];
						const auto& transformB = view.get<TransformComponent>(entityB);
						const auto& colliderB = view.get<BoxColliderComponent>(entityB);
						glm::vec2 posB = transformB.position + colliderB.offset;
						glm::vec2 sizeB = colliderB.size;

						bool collided =
							posA.x < posB.x + sizeB.x &&
							posA.x + sizeA.x > posB.x &&
							posA.y < posB.y + sizeB.y &&
							posA.y + sizeA.y > posB.y;

						if (collided) {
							glm::vec2 overlap(
								std::min(posA.x + sizeA.x, posB.x + sizeB.x) - std::max(posA.x, posB.x),
								std::min(posA.y + sizeA.y, posB.y + sizeB.y) - std::max(posA.y, posB.y)
							);

							dispatcher.enqueue<CollisionEvent>({ entityA, entityB, overlap });
						}
					}
				}
			}
		}
	}
};

class CollisionResolutionSystem : public ISystem {
private:
	entt::registry& registry;

public:
	CollisionResolutionSystem(entt::registry& reg, entt::dispatcher& dispatcher)
		: registry(reg) {
		dispatcher.sink<CollisionEvent>().connect<&CollisionResolutionSystem::OnCollision>(*this);
	}

	void OnCollision(const CollisionEvent& event) {
		auto entityA = event.a;
		auto entityB = event.b;

		bool aIsPlayer = registry.any_of<PlayerTag>(entityA);
		bool bIsPlayer = registry.any_of<PlayerTag>(entityB);

		bool aIsNPC = registry.any_of<NPCTag>(entityA);
		bool bIsNPC = registry.any_of<NPCTag>(entityB);

		// Player vs NPC
		if (aIsPlayer && bIsNPC) {
			auto& transform = registry.get<TransformComponent>(entityA);
			transform.position = transform.previousPosition; // stop player
		}
		else if (bIsPlayer && aIsNPC) {
			auto& transform = registry.get<TransformComponent>(entityB);
			transform.position = transform.previousPosition; // stop player
		}
		// Player vs Wall (or any static object)
		else if (aIsPlayer) {
			auto& transform = registry.get<TransformComponent>(entityA);
			transform.position = transform.previousPosition;
		}
		else if (bIsPlayer) {
			auto& transform = registry.get<TransformComponent>(entityB);
			transform.position = transform.previousPosition;
		}
		// Pushable objects (future)
		else if (registry.any_of<PushableComponent>(entityA) && aIsPlayer) {
			// allow push
		}
		else if (registry.any_of<PushableComponent>(entityB) && bIsPlayer) {
			// allow push
		}
	}
};