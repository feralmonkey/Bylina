#pragma once
#include <sol/sol.hpp>
struct ScriptComponent {
	sol::function func;

	explicit ScriptComponent(const sol::function& func = sol::lua_nil) {
		this->func = func;
	}
};