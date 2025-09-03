#pragma once
#include <cstdint>
#include <string>
#include <array>
#include <glm/glm.hpp>
#include <spdlog.h>


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

struct BoxColliderComponent {
	int width;
	int height;
	glm::vec2 offset;

	BoxColliderComponent(int width = 32, int height = 32, glm::vec2 offset = glm::vec2(0)) {
		this->width = width;
		this->height = height;
		this->offset = offset;
	}
};


struct CameraFollowComponent {
	CameraFollowComponent() = default;
};

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

struct KeyboardControlComponent {
	glm::vec2 upVelocity;
	glm::vec2 rightVelocity;
	glm::vec2 downVelocity;
	glm::vec2 leftVelocity;

	KeyboardControlComponent(glm::vec2 upVelocity = glm::vec2(0), glm::vec2 rightVelocity = glm::vec2(0), glm::vec2 downVelocity = glm::vec2(0), glm::vec2 leftVelocity = glm::vec2(0)) {
		this->upVelocity = upVelocity;
		this->rightVelocity = rightVelocity;
		this->downVelocity = downVelocity;
		this->leftVelocity = leftVelocity;
	}
};

struct ScriptComponent {
	sol::function func;

	ScriptComponent(sol::function func = sol::lua_nil) {
		this->func = func;
	}
};

struct TextComponent {
	glm::vec2 position;
	std::string text;
	std::string assetId;
	SDL_Color color;
	bool isFixed;

	TextLabelComponent(glm::vec2 position = glm::vec2(0), std::string text = "", std::string assetId = "", const SDL_Color color = { 0, 0, 0 }, bool isFixed = true) {
		this->position = position;
		this->text = text;
		this->assetId = assetId;
		this->color = color;
		this->isFixed = isFixed;
	}
};

// to replace with tagging system
// Simple tag for the player
struct Player {};

