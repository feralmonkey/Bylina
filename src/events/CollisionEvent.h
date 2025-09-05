#pragma once
#include "entt.hpp"
#include "../events/EventBus.h"
#include "Event.h"

// UPDATED Entity to entt:entity. Super sus that this will work! Changed the include from the old ECS.
class CollisionEvent : public Event {
	public:
		entt::entity a;
		entt::entity b;
		CollisionEvent(entt::entity a, entt::entity b) : a(a), b(b) {}
};