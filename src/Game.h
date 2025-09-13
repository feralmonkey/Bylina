#pragma once
#include "../src/assetstore/AssetStore.h"
#include "systems/ScriptSystem.h"
#include "systems/KeyboardControlSystem.h"
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

	entt::registry registry; // on the fence about exposing these as public
	entt::dispatcher dispatcher;
	KeyboardControlSystem keyboardSystem;

	static int windowScale;
	static int logicalWidth;
	static int logicalHeight;
	static int mapWidth;
	static int mapHeight;
};