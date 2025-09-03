#pragma once
#include <cstdint>
#include <string>
#include <array>
#include <glm/glm.hpp>
#include <spdlog.h>

// to replace
struct Position { int x{ 0 }, y{ 0 }; };
// to replace
struct Velocity { int dx{ 0 }, dy{ 0 }; };

struct TransformComponent {
	glm::vec2 position;
	glm::vec2 scale;
	double rotation;

	TransformComponent(glm::vec2 position = glm::vec2(0, 0), glm::vec2 scale = glm::vec2(1, 1), double rotation = 0.0) {
		this->position = position;
		this->scale = scale;
		this->rotation = rotation;
	}
};

struct RigidBodyComponent {
	glm::vec2 velocity;

	RigidBodyComponent(glm::vec2 velocity = glm::vec2(0.0, 0.0)) {
		this->velocity = velocity;
	}
};

// 16x16 NES-ish tile index (palette-free for now)
// to replace
struct Sprite {
    int tileIndex{ 0 };
    int w{ 16 };
    int h{ 16 };
};

// Sprite Component
struct SpriteComponent {
public:
	std::string assetId;
	int width;
	int height;
	int zIndex;
	SDL_RendererFlip flip;
	SDL_Rect srcRect;
	bool isFixed;

	SpriteComponent(std::string assetId = "", int width = 0, int height = 0, int zIndex = 0, bool isFixed = false, int srcRectX = 0, int srcRectY = 0) {
		this->assetId = assetId;
		this->width = width;
		this->height = height;
		this->zIndex = zIndex;
		this->flip = SDL_FLIP_NONE;
		this->isFixed = isFixed;
		this->srcRect = { srcRectX, srcRectY, width, height };
	};

	bool compareZindex(const SpriteComponent& a, const SpriteComponent& b) {
		return a.zIndex < b.zIndex;
	}
};

// to replace with tagging system
// Simple tag for the player
struct Player {};

// to replace
// Map coordinates + solid flag for collisions later
struct Collider { bool solid{ true }; };

// to replace
// Script hook id
struct ScriptRef { std::string onInteract; };

// to replace
// Camera follows a target entity
struct CameraFollow {};

struct AnimationComponent {
	int numFrames;
	int currentFrame;
	int frameSpeedRate;
	int startTime;
	bool isLoop;

	AnimationComponent(int numFrames = 1, int frameSpeedRate = 1, bool isLoop = true) {
		this->numFrames = numFrames;
		this->currentFrame = 1;
		this->frameSpeedRate = frameSpeedRate;
		this->isLoop = isLoop;
		this->startTime = SDL_GetTicks();
	}
};

// all below to replace
struct KeyboardControlComponent { int i = 0; };
