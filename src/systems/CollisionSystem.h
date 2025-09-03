#pragma once

#include "../components/TransformComponent.h"
#include "../components/BoxColliderComponent.h"
#include <spdlog/spdlog.h> 
#include <algorithm>
#include "../events/EventBus.h"
#include "../events/CollisionEvent.h"

class CollisionSystem : public System {
	public:
		CollisionSystem() {
			RequireComponent<BoxColliderComponent>();
			RequireComponent<TransformComponent>();
		}

		void Update(std::unique_ptr<EventBus>& eventBus) {
			bool hasCollided;
			std::vector<Entity> entities = GetSystemEntities();
			std::vector<Entity> processedEntities;

			for (int i = 0; i < entities.size(); i++) {
				for (int j = i + 1; j < entities.size(); j++) {

					auto offset1 = entities[i].GetComponent<BoxColliderComponent>().offset;
					auto offset2 = entities[j].GetComponent<BoxColliderComponent>().offset;
					
					int left1 = entities[i].GetComponent<TransformComponent>().position.x + offset1.x;
					int right1 = entities[i].GetComponent<TransformComponent>().position.x + offset1.x + entities[i].GetComponent<BoxColliderComponent>().width;
					int top1 = entities[i].GetComponent<TransformComponent>().position.y + offset1.y;
					int bottom1 = entities[i].GetComponent<TransformComponent>().position.y + offset1.y + entities[i].GetComponent<BoxColliderComponent>().height;

					int left2 = entities[j].GetComponent<TransformComponent>().position.x + offset2.x;
					int right2 = entities[j].GetComponent<TransformComponent>().position.x + offset2.x +  entities[j].GetComponent<BoxColliderComponent>().width;
					int top2 = entities[j].GetComponent<TransformComponent>().position.y + offset2.y;
					int bottom2 = entities[j].GetComponent<TransformComponent>().position.y + offset2.y + entities[j].GetComponent<BoxColliderComponent>().height;

					// check to see if a collision has occured
					if (right1 >= left2 && left1 <= right2 && bottom1 >= top2 && top1 <= bottom2) {
						
						// for logging - causes a lot of noise otherwise
						//spdlog::info("Entity " + std::to_string(entities[i].GetId()) + " is colliding with " + std::to_string(entities[j].GetId()));

						processedEntities.emplace_back(entities[i]);
						processedEntities.emplace_back(entities[j]);
						
						eventBus->EmitEvent<CollisionEvent>(entities[i], entities[j]);
					}
				}
			}
		}
};