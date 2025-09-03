#pragma once


#include <SDL.h>
#include "../ecs/ecs.h"
#include <spdlog/spdlog.h>
#include "../components/CameraFollowComponent.h"

class CameraMovementSystem : public System {
	public:
		CameraMovementSystem() {
			RequireComponent<CameraFollowComponent>();
			RequireComponent<TransformComponent>();
		}

		void Update(SDL_Rect& camera) {
			for (auto entity: GetSystemEntities()) {
				auto transform = entity.GetComponent<TransformComponent>();

				if (transform.position.x + (camera.w / 2) < Game::mapWidth) {
					camera.x = transform.position.x - (Game::windowWidth / 2);
				}

				if (transform.position.y + (camera.h / 2) < Game::mapHeight) {
					camera.y = transform.position.y - (Game::windowHeight / 2);
				}

				// keep camera view inside screen limits
				camera.x = camera.x < 0 ? 0 : camera.x;
				camera.y = camera.y < 0 ? 0 : camera.y;
				camera.x = (camera.x + camera.w > Game::mapWidth) ? Game::mapWidth - camera.w : camera.x;
				camera.y = (camera.y + camera.h > Game::mapHeight) ? Game::mapHeight - camera.h : camera.y;
			}
		}
};