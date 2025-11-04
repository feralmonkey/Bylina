#pragma once

struct PlayerComponent {
	bool isActive = true;
	
	explicit PlayerComponent(const bool isActive = true) {
		this->isActive = isActive;
	}
};