#pragma once
#include "entt.hpp"

struct MenuNavigateEvent {
    enum class Direction { Up, Down, Confirm, Cancel };
    Direction dir;
    entt::entity menuEntity;
};