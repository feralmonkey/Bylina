#pragma once

#include <entt.hpp>
#include <spdlog/spdlog.h>
#include "../components/BoxColliderComponent.h"
#include "../components/TransformComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../events/CollisionEvent.h"

class CollisionSystem {
private:
	entt::registry& registry;
	entt::dispatcher& dispatcher;
	int tileSize;

public:
	CollisionSystem(entt::registry& reg, entt::dispatcher& dis, int tileSize) 
		: registry(reg), dispatcher(dis), tileSize(tileSize)   {}
	
	void Update(int mapWidth, int mapHeight) {
		int mapCols = mapWidth / tileSize;
		int mapRows = mapHeight / tileSize;


		// Grid (each cell contains entities)
		std::vector<std::vector<std::vector<entt::entity>>> grid(
			mapRows, std::vector<std::vector<entt::entity>>(mapCols)
		);

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
					grid[r][c].push_back(entity);
				}
			}
		}

		// Check collisions inside each tile
		for (int r = 0; r < mapRows; ++r) {
			for (int c = 0; c < mapCols; ++c) {
				auto& cellEntities = grid[r][c];

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
							// --- Resolve overlap ---
							glm::vec2 overlap(
								std::min(posA.x + sizeA.x, posB.x + sizeB.x) - std::max(posA.x, posB.x),
								std::min(posA.y + sizeA.y, posB.y + sizeB.y) - std::max(posA.y, posB.y)
							);

							if (overlap.x < overlap.y) {
								// Push along X axis
								if (posA.x < posB.x) {
									registry.get<TransformComponent>(entityA).position.x -= overlap.x;
								}
								else {
									registry.get<TransformComponent>(entityA).position.x += overlap.x;
								}
							}
							else {
								// Push along Y axis
								if (posA.y < posB.y) {
									registry.get<TransformComponent>(entityA).position.y -= overlap.y;
								}
								else {
									registry.get<TransformComponent>(entityA).position.y += overlap.y;
								}
							}

							// Stop velocity if entity has one
							if (registry.any_of<RigidBodyComponent>(entityA)) {
								registry.get<RigidBodyComponent>(entityA).velocity = { 0, 0 };
							}

							dispatcher.enqueue<CollisionEvent>({ entityA, entityB });
						}
					}
				}
			}
		}
	}
};