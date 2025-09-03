#pragma once

#include "../ecs/ecs.h"
#include "../components/ScriptComponent.h"
#include "../components/TransformComponent.h"
#include "../components/RigidBodyComponent.h"
#include "../components/AnimationComponent.h"
#include <spdlog/spdlog.h> 
#include <tuple>

// first declare some native c++ functions that we will bind with lua functions

std::tuple<double, double> GetEntityPosition(Entity entity) {
	if (entity.HasComponent<TransformComponent>()) {
		const TransformComponent transform = entity.GetComponent<TransformComponent>();
		return std::make_tuple(transform.position.x, transform.position.y);
	}
	else {
		spdlog::error("trying to get the position of an entity that has no transform component");
		return std::make_tuple(0.0, 0.0);
	}
}

std::tuple<double, double> GetEntityVelocity(Entity entity) {
	if (entity.HasComponent<RigidBodyComponent>()) {
		const RigidBodyComponent rigidBody = entity.GetComponent<RigidBodyComponent>();
		return std::make_tuple(rigidBody.velocity.x, rigidBody.velocity.y);
	}
	else {
		spdlog::error("trying to get the velocity of an entity that has no rigid body component");
		return std::make_tuple(0.0, 0.0);
	}
}

void SetEntityPosition(Entity entity, double x, double y) {
	if (entity.HasComponent<TransformComponent>()) {
		TransformComponent& transform = entity.GetComponent<TransformComponent>();
		transform.position.x = x;
		transform.position.y = y;
	}
	else {
		spdlog::error("trying to set the position of an entity that has no transform component");
	}
}

void SetEntityVelocity(Entity entity, double x, double y) {
	if (entity.HasComponent<RigidBodyComponent>()) {
		RigidBodyComponent& rigidBody = entity.GetComponent<RigidBodyComponent>();
		rigidBody.velocity.x = x;
		rigidBody.velocity.y = y;
	}
	else {
		spdlog::error("trying to set the velocity of an entity that has no rigid body component");
	}
}

void SetEntityRotation(Entity entity, double angle) {
	if (entity.HasComponent<TransformComponent>()) {
		TransformComponent& transform = entity.GetComponent<TransformComponent>();
		transform.rotation = angle;
	}
	else {
		spdlog::error("trying to set the rotation of an entity that has no transform component");
	}
}

void SetEntityAnimationFrame(Entity entity, int frame) {
	if (entity.HasComponent<AnimationComponent>()) {
		AnimationComponent& animation = entity.GetComponent<AnimationComponent>();
		animation.currentFrame = frame;
	}
	else {
		spdlog::error("trying to set the animation frame of an entity that has no animation component");
	}
}

void SetProjectileVelocity(Entity entity, double x, double y) {
	if (entity.HasComponent<ProjectileEmitterComponent>()) {
		ProjectileEmitterComponent& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
		projectileEmitter.projectileVelocity.x = x;
		projectileEmitter.projectileVelocity.y = y;
	}
	else {
		spdlog::error("trying to set the projectile velocity of an entity that has no projectile emitter component");
	}
}

class ScriptSystem : public System {
public:

	class ScriptSystem() {
		RequireComponent<ScriptComponent>();
	}

	void CreateLuaBindings(sol::state& lua) {
		// create the 'entity' user type so lua knows what an entity is and
		// expose methods we will use in lua
		lua.new_usertype<Entity>(
			"entity",
			"get_id", &Entity::GetId,
			"destroy", &Entity::Kill,
			"has_tag", &Entity::HasTag,
			"belongs_to_group", &Entity::BelongsToGroup
		);

		// create all the bindings between c++ and lua functions
		lua.set_function("get_position", GetEntityPosition);
		lua.set_function("get_velocity", GetEntityVelocity);
		lua.set_function("set_position", SetEntityPosition);
		lua.set_function("set_velocity", SetEntityVelocity);
		lua.set_function("set_rotation", SetEntityRotation);
		lua.set_function("set_projectile_velocity", SetProjectileVelocity);
		lua.set_function("set_animation_frame", SetEntityAnimationFrame);

		// create the bindings between c++ and lua functions
		lua.set_function("set_position", SetEntityPosition);
	}

	void Update(double deltaTime, int ellapsedTime) {
		// loop all entities that have a script component and invoke their lua function
		for (auto entity : GetSystemEntities()) {
			const auto& script = entity.GetComponent<ScriptComponent>();
			script.func(entity, deltaTime, ellapsedTime); // here is where we invoke sol::func
		}
	}
};
