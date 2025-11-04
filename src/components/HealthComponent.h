#pragma once

struct HealthComponent {
public:
	int healthPercentage;

	explicit HealthComponent(const int healthPercentage = 1) {
		this->healthPercentage = healthPercentage;
	}
};