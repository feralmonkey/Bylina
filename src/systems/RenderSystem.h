#pragma once

#include "../ecs/ecs.h"
#include "../components/TransformComponent.h"
#include "../components/SpriteComponent.h"
#include "../assetstore/AssetStore.h"
#include <spdlog/spdlog.h> 
#include <SDL.h>

class RenderSystem : public System {
	public: 
		RenderSystem() {
			RequireComponent<TransformComponent>();
			RequireComponent<SpriteComponent>();
		}

		void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect& camera) {
			// create a vector with both sprite and transform components of all entities
			struct RenderableEntity {
				TransformComponent transformComponent;
				SpriteComponent spriteComponent;
			};
			
			std::vector<RenderableEntity> renderableEntities;

			for (auto entity : GetSystemEntities()) {
				RenderableEntity renderableEntity;
				renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
				renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();

				// margin to continue rendering sprite if it falls outside of the camera
				int cameraMargin = 100;

				// is the entity outside of the camera view...?
				bool isEntityOutsideCameraView = (
					renderableEntity.transformComponent.position.x + (renderableEntity.transformComponent.scale.x * renderableEntity.spriteComponent.width) < camera.x - cameraMargin ||
					renderableEntity.transformComponent.position.x > camera.x + camera.w + cameraMargin ||
					renderableEntity.transformComponent.position.y + (renderableEntity.transformComponent.scale.y * renderableEntity.spriteComponent.height) < camera.y - cameraMargin ||
					renderableEntity.transformComponent.position.y > camera.y + camera.h + cameraMargin
				);

				// ...if so, bypass rendering
				if (isEntityOutsideCameraView && !renderableEntity.spriteComponent.isFixed) {
					continue;
				}

				renderableEntities.emplace_back(renderableEntity);
			}

			// sort all entities of render system by zIndex
			std::sort(renderableEntities.begin(), renderableEntities.end(), 
				[](const RenderableEntity& a, const RenderableEntity &b){
					return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
			});

			// loop all entities the system is interested in
			for (auto entity: renderableEntities) {
				// update entity position based on it's velocity
				const auto transform = entity.transformComponent;
				const auto sprite = entity.spriteComponent;
				
				// set the source rectangle of our original sprite texture
				SDL_Rect srcRect = sprite.srcRect;

				SDL_Rect dstRect = {
					static_cast<int>(transform.position.x - (sprite.isFixed ? 0 : camera.x)),
					static_cast<int>(transform.position.y - (sprite.isFixed ? 0 : camera.y)),
					static_cast<int>(sprite.width * transform.scale.x),
					static_cast<int>(sprite.height * transform.scale.y)
				};

				// Draw the texture on the destination renderer
				SDL_RenderCopyEx(
					renderer,
					assetStore->GetTexture(sprite.assetId),
					&srcRect,
					&dstRect,
					transform.rotation,
					NULL, // rotation will be done around the center by default
					sprite.flip
				);
			}
		}
};