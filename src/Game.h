#pragma once
#include "../src/assetstore/AssetStore.h"
#include "systems/ScriptSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/KeyboardControlSystem.h"
#include "systems/RenderTextSystem.h"
#include "systems/MovementSystem.h"
#include "enums/InputState.h"
#include <SDL.h>
#include <sol/sol.hpp>
#include <entt.hpp>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {
private:
	bool gameIsRunning;
	bool debugMode;
	int millisecondsPreviousFrame;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Rect camera;
	sol::state lua;
	entt::dispatcher dispatcher;
	entt::registry registry;

	std::vector<InputState> inputStack;

	KeyboardControlSystem keyboardSystem;
	RenderTextSystem textSystem;
	MovementSystem movementSystem;
	CollisionSystem collisionSystem;

	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<ScriptSystem> scriptSystem;


public:
	Game();
	~Game();
	void Initialize();
	void Setup();
	void Run();
	void ProcessInput();
	void Update();
	void Render();
	void Destroy();

	static int windowScale;
	static int logicalWidth;
	static int logicalHeight;
	static int mapWidth;
	static int mapHeight;
};