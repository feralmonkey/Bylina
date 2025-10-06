#pragma once

#include "entt.hpp"
#include "../systems/ISystem.h"
#include "../components/NPCComponent.h"
#include "../components/ScriptComponent.h"
#include "../components/TransformComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/AnimationComponent.h"
#include <spdlog/spdlog.h> 
#include <tuple>

// first declare some native c++ functions that we will bind with lua functions
inline std::tuple<double, double> GetEntityPosition(entt::registry& reg, entt::entity entity) {
	if (reg.any_of<TransformComponent>(entity)) {
		const TransformComponent transform = reg.get<TransformComponent>(entity);
		return std::make_tuple(transform.position.x, transform.position.y);
	}
	else {
		spdlog::error("trying to get the position of an entity that has no transform component");
		return std::make_tuple(0.0, 0.0);
	}
}

inline std::tuple<double, double> GetEntityVelocity(entt::registry& reg, entt::entity entity) {
	if (reg.any_of<RigidBodyComponent>(entity)) {
		const RigidBodyComponent rigidBody = reg.get<RigidBodyComponent>(entity);
		return std::make_tuple(rigidBody.velocity.x, rigidBody.velocity.y);
	}
	else {
		spdlog::error("trying to get the velocity of an entity that has no rigid body component");
		return std::make_tuple(0.0, 0.0);
	}
}

inline void SetEntityPosition(entt::registry& reg, entt::entity entity, double x, double y) {
	if (reg.any_of<TransformComponent>(entity)) {
		TransformComponent& transform = reg.get<TransformComponent>(entity);
		transform.position.x = x;
		transform.position.y = y;
	}
	else {
		spdlog::error("trying to set the position of an entity that has no transform component");
	}
}

inline void SetEntityVelocity(entt::registry& reg, entt::entity entity, double x, double y) {
  if (reg.any_of<RigidBodyComponent>(entity)) {
		RigidBodyComponent& rigidBody = reg.get<RigidBodyComponent>(entity);
		rigidBody.velocity.x = x;
		rigidBody.velocity.y = y;
	}
	else {
		spdlog::error("trying to set the velocity of an entity that has no rigid body component");
	}
}

inline void SetEntityRotation(entt::registry& reg, entt::entity entity, double angle) {
  if (reg.any_of<TransformComponent>(entity)) {
		TransformComponent& transform = reg.get<TransformComponent>(entity);
		transform.rotation = angle;
	}
	else {
		spdlog::error("trying to set the rotation of an entity that has no transform component");
	}
}

inline void SetEntityAnimationFrame(entt::registry& reg, entt::entity entity, int frame) {
  if (reg.any_of<AnimationComponent>(entity)) {
		AnimationComponent& animation = reg.get<AnimationComponent>(entity);
		animation.currentFrame = frame;
	}
	else {
		spdlog::error("trying to set the animation frame of an entity that has no animation component");
	}
}

class ScriptSystem : public ISystem {
public:

	ScriptSystem() = default;
	
	void CreateLuaBindings(sol::state& lua) {
		// create the 'entity' user type so lua knows what an entity is
		lua.new_usertype<entt::entity>(
			"entity"
		);
		lua.new_usertype<NPCComponent>("NPCComponent",
			sol::constructors<
			NPCComponent(std::string,
				std::unordered_map<std::string, std::string>,
				MovementPattern,
				MovementSpeed)>(),
			"name", &NPCComponent::name,
			"conversation", &NPCComponent::conversation,
			"movementPattern", &NPCComponent::movementPattern,
			"speed", &NPCComponent::speed
		);

		// create the 'registry' user type so lua knows what a registry is...
		// ...and expose methods we will use in lua
		lua.new_usertype<entt::registry>(
			"registry",
			// create a new entity
			"create", [](entt::registry& r) {
				return r.create();
			},
			// get entity id
			"get_id", [](entt::registry&, entt::entity e) {
				return static_cast<entt::id_type>(entt::to_integral(e));
			},
			// check if the entity is valid
			"valid", [](entt::registry& r, entt::entity e) {
				return r.valid(e);
			},
			// destroy an entity (returns false if it was an invalid entity)
			"destroy", [](entt::registry& r, entt::entity e) {
				bool alive = r.valid(e);
				r.destroy(e);
				return alive;
			}
		);

		// configure LUA enums
		lua.new_enum("MovementPattern",
			"Still", MovementPattern::Still,
			"Box", MovementPattern::Box,
			"Random", MovementPattern::Random,
			"Line", MovementPattern::Line
		);

		lua.new_enum("MovementSpeed",
			"Slow", MovementSpeed::Slow,
			"Normal", MovementSpeed::Normal,
			"Fast", MovementSpeed::Fast
		);

		// create all the bindings between c++ and lua functions
		lua.set_function("get_position", GetEntityPosition);
		lua.set_function("get_velocity", GetEntityVelocity);
		lua.set_function("set_position", SetEntityPosition);
		lua.set_function("set_velocity", SetEntityVelocity);
		lua.set_function("set_rotation", SetEntityRotation);
		lua.set_function("set_animation_frame", SetEntityAnimationFrame);

		// create the bindings between c++ and lua functions
		lua.set_function("set_position", SetEntityPosition);
	}

	void Update(entt::registry& reg, double deltaTime, int ellapsedTime) {
		// loop all entities that have a script component and invoke their lua function
		auto view = reg.view<ScriptComponent>();
		for (auto entity : view) {
			const auto& script = view.get<ScriptComponent>(entity);
			script.func(entity, deltaTime, ellapsedTime); // here is where we invoke sol::func
		}
	}
};
