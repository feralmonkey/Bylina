#pragma once
#include "../src/assetstore/AssetStore.h"
#include "../src/events/EventBus.h"
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
	

	entt::registry registry;
	std::unique_ptr<AssetStore> assetStore; // there may be an assetstore alternative in ett
	std::unique_ptr<EventBus> eventBus; // there may be an event bus alternative in ett

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

	static int windowWidth;
	static int windowHeight;
	static int mapWidth;
	static int mapHeight;
};