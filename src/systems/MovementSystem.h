#pragma once

#include "../events/CollisionEvent.h"
#include "../components/KeyboardControlComponent.h"
#include "../components/SpriteComponent.h"
#include <SDL.h>
#include <spdlog/spdlog.h> 
#include <entt.hpp>


class MovementSystem {
private:
	entt::registry& registry;

public:
	MovementSystem(entt::registry& reg) : registry(reg) {}

	void Update(double deltaTime) {
		// loop all enteties that the system is interested in
		auto view = registry.view<SpriteComponent, RigidBodyComponent, TransformComponent>();
		for (auto entity : view) {
			// update entity position based on it's velocity
			TransformComponent& transform = view.get<TransformComponent>(entity);
			RigidBodyComponent rigidBody = view.get<RigidBodyComponent>(entity);
			SpriteComponent sprite = view.get<SpriteComponent>(entity);

			transform.position.x += rigidBody.velocity.x;// * deltaTime;
			transform.position.y += rigidBody.velocity.y;// * deltaTime;

			// keeping this around since it might come into play later
			//if (auto* player = registry.try_get<PlayerComponent>(entity)) { }
		}
	}

	void OnCollision(const CollisionEvent& event) {
		std::cout << "COLLISION" << std::endl;
		//entt::entity a = event.a;
		//entt::entity b = event.b;
	}
};