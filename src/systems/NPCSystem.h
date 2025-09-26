#pragma once

#include <iostream>
#include <entt.hpp>
#include "../components/NPCComponent.h"
#include "../enums/NPCMovement.h"
#include "../events/TalkEvent.h"


class NPCSystem {
private:
	entt::registry& registry;
	entt::dispatcher& dispatcher;

	void onTalk() {

	}

	void MoveRandom() {
		std::cout << "random" << std::endl;
	}

public:

	NPCSystem(entt::registry& reg, entt::dispatcher& dis) :
		dispatcher(dis),
		registry(reg)
	{
		//dispatcher.sink<TalkEvent>().connect<&NPCSystem::onTalk>(*this); //todo rigolo will need to deal with this
	}

	void Update() {
		auto view = registry.view<NPCComponent>();
		int i = 0;
		for (entt::entity entity : view) {
			const auto& pattern = view.get<NPCComponent>(entity).movementPattern;
			const auto& speed = view.get<NPCComponent>(entity).speed;
			
			auto& turn = view.get<NPCComponent>(entity).framesSinceLastAction;

			// todo rigolo -- this is not good, should be actual time and not a solid number
			int frameMax = 120;
			if (speed == MovementSpeed::Fast) {  // todo -- handle fast logic later
				frameMax = 60;
			}
			else if (speed == MovementSpeed::Normal) {
				frameMax = 120;
			}
			else if (speed == MovementSpeed::Slow) { // todo rigolo -- handle slow logic later
				frameMax = 180;
			}


			if (turn >= frameMax) {
				turn = 0; // reset turn
			}
			else {
				turn++;
				return;
			}

			if (pattern == MovementPattern::Random) {
				MoveRandom();
			}
		}
	}
};