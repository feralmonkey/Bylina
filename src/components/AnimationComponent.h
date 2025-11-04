#pragma once

#include <SDL.h>

struct AnimationComponent {
	int numFrames;
	int currentFrame;
	int frameSpeedRate;
	int startTime;
	bool isLoop;

	explicit AnimationComponent(const int numFrames = 1, const int frameSpeedRate = 1, const bool isLoop = true) {
		this->numFrames = numFrames;
		this->currentFrame = 1;
		this->frameSpeedRate = frameSpeedRate;
		this->isLoop = isLoop;
		this->startTime = static_cast<int>(SDL_GetTicks());
	}
};
