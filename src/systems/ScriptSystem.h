#pragma once

#include "entt.hpp"
#include "../components/ScriptComponent.h"
#include "../components/TransformComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/AnimationComponent.h"
#include <spdlog/spdlog.h> 
#include <tuple>

// first declare some native c++ functions that we will bind with lua functions
std::tuple<double, double> GetEntityPosition(entt::registry& reg, entt::entity entity) {
	if (reg.any_of<TransformComponent>(entity)) {
		const TransformComponent transform = reg.get<TransformComponent>(entity);
		return std::make_tuple(transform.position.x, transform.position.y);
	}
	else {
		spdlog::error("trying to get the position of an entity that has no transform component");
		return std::make_tuple(0.0, 0.0);
	}
}

std::tuple<double, double> GetEntityVelocity(entt::registry& reg, entt::entity entity) {
	if (reg.any_of<RigidBodyComponent>(entity)) {
		const RigidBodyComponent rigidBody = reg.get<RigidBodyComponent>(entity);
		return std::make_tuple(rigidBody.velocity.x, rigidBody.velocity.y);
	}
	else {
		spdlog::error("trying to get the velocity of an entity that has no rigid body component");
		return std::make_tuple(0.0, 0.0);
	}
}

void SetEntityPosition(entt::registry& reg, entt::entity entity, double x, double y) {
	if (reg.any_of<TransformComponent>(entity)) {
		TransformComponent& transform = reg.get<TransformComponent>(entity);
		transform.position.x = x;
		transform.position.y = y;
	}
	else {
		spdlog::error("trying to set the position of an entity that has no transform component");
	}
}

void SetEntityVelocity(entt::registry& reg, entt::entity entity, double x, double y) {
  if (reg.any_of<RigidBodyComponent>(entity)) {
		RigidBodyComponent& rigidBody = reg.get<RigidBodyComponent>(entity);
		rigidBody.velocity.x = x;
		rigidBody.velocity.y = y;
	}
	else {
		spdlog::error("trying to set the velocity of an entity that has no rigid body component");
	}
}

void SetEntityRotation(entt::registry& reg, entt::entity entity, double angle) {
  if (reg.any_of<TransformComponent>(entity)) {
		TransformComponent& transform = reg.get<TransformComponent>(entity);
		transform.rotation = angle;
	}
	else {
		spdlog::error("trying to set the rotation of an entity that has no transform component");
	}
}

void SetEntityAnimationFrame(entt::registry& reg, entt::entity entity, int frame) {
  if (reg.any_of<AnimationComponent>(entity)) {
		AnimationComponent& animation = reg.get<AnimationComponent>(entity);
		animation.currentFrame = frame;
	}
	else {
		spdlog::error("trying to set the animation frame of an entity that has no animation component");
	}
}

class ScriptSystem {
public:

	class ScriptSystem() {
		/*RequireComponent<ScriptComponent>();*/
	}

	void CreateLuaBindings(sol::state& lua) {
		// create the 'entity' user type so lua knows what an entity is and
		// expose methods we will use in lua
		//lua.new_usertype<entt::entity>(
		//	"entity",
		//	"get_id", &entt::registry::get,
		//	"destroy", &entt::registry::destroy
			//"has_tag", &entt::registry::
			//"belongs_to_group", &entt::registry::BelongsToGroup
		//);

		//void CreateLuaBindings(sol::state & lua) {
		//	// create the 'entity' user type so lua knows what an entity is and
		//	// expose methods we will use in lua
		//	lua.new_usertype<Entity>(
		//		"entity",
		//		"get_id", &Entity::GetId,
		//		"destroy", &Entity::Kill,
		//		"has_tag", &Entity::HasTag,
		//		"belongs_to_group", &Entity::BelongsToGroup
		//	);

		//// create all the bindings between c++ and lua functions
		//lua.set_function("get_position", GetEntityPosition);
		//lua.set_function("get_velocity", GetEntityVelocity);
		//lua.set_function("set_position", SetEntityPosition);
		//lua.set_function("set_velocity", SetEntityVelocity);
		//lua.set_function("set_rotation", SetEntityRotation);
		//lua.set_function("set_projectile_velocity", SetProjectileVelocity);
		//lua.set_function("set_animation_frame", SetEntityAnimationFrame);

		//// create the bindings between c++ and lua functions
		//lua.set_function("set_position", SetEntityPosition);
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
