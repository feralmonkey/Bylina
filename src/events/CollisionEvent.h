#pragma once
#include "../ecs/ecs.h"
#include "../events/EventBus.h"
#include "Event.h"

class CollisionEvent : public Event {
	public:
		Entity a;
		Entity b;
		CollisionEvent(Entity a, Entity b) : a(a), b(b) {}
};