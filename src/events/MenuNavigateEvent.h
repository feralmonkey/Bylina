#pragma once
#include "entt.hpp"

struct MenuNavigateEvent {
    enum class MenuDirection { Up, Down, Confirm, Cancel };
    MenuDirection dir;
    entt::entity menuEntity;
};