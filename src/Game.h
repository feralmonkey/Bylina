#pragma once

#include <SDL.h>
#include <sol/sol.hpp>
#include <entt.hpp>
#include <memory>
#include <vector>
//#include "Constants.h"
#include "enums/InputState.h"

// Forward declarations
class AssetStore;
class RenderTextSystem;
class KeyboardControlSystem;
class MovementSystem;
class CollisionSystem;
class CollisionResolutionSystem;
class MenuSystem;
class NPCSystem;
class ScriptSystem;

class Game {
private:
	bool gameIsRunning;
	bool debugMode;
	uint millisecondsPreviousFrame;

	SDL_Window* window {};
	SDL_Renderer* renderer {};
	SDL_Rect camera {};

	sol::state lua;
	entt::dispatcher dispatcher;
	entt::registry registry;

	std::vector<InputState> inputStack;
	std::unique_ptr<AssetStore> assetStore;

	// Use smart pointers for systems
	std::unique_ptr<RenderTextSystem> textSystem;
	std::unique_ptr<KeyboardControlSystem> keyboardSystem;
	std::unique_ptr<MovementSystem> movementSystem;
	std::unique_ptr<CollisionSystem> collisionSystem;
	std::unique_ptr<CollisionResolutionSystem> collisionResolutionSystem;
	std::unique_ptr<MenuSystem> menuSystem;
	std::unique_ptr<NPCSystem> npcSystem;
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
	void Destroy() const;

	static int windowScale;
	static int logicalWidth;
	static int logicalHeight;
	static int mapWidth;
	static int mapHeight;
};