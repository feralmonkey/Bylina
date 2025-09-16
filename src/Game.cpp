#include "Game.h"
#include "MapLoader.h"
#include "components/TransformComponent.h"
#include "components/RigidBodyComponent.h"
#include "components/SpriteComponent.h"
#include "components/AnimationComponent.h"
#include "components/PlayerComponent.h"
#include "components/TextComponent.h"
#include "components/CameraFollowComponent.h"
#include "events/KeyPressedEvent.h"
#include "systems/RenderSystem.h"
#include "systems/AnimationSystem.h"
#include "systems/KeyboardControlSystem.h"
#include "systems/MovementSystem.h"
#include "systems/RenderTextSystem.h"
#include "systems/CameraMovementSystem.h"


// initialize static member variables
int Game::logicalWidth;
int Game::logicalHeight;
int Game::windowScale;
int Game::mapWidth;
int Game::mapHeight;

Game::Game() : keyboardSystem(registry, dispatcher)
{
	spdlog::info("Game constructor called!");
	Game::gameIsRunning = false;
	debugMode = false;

	assetStore = std::make_unique<AssetStore>();
	scriptSystem = std::make_unique<ScriptSystem>();
}

Game::~Game() {
	spdlog::info("Game destructor called!");
}

void Game::Initialize() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		spdlog::error("Error Initializing SDL");
		return;
	}

	if (TTF_Init() != 0) {
		spdlog::error("Error Initializing SDL TTF");
	}

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	
	windowScale = 3;
	logicalWidth = 256;
	logicalHeight = 240;

	// Create window
	SDL_Window* window = SDL_CreateWindow(
		"Bylina", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		logicalWidth * windowScale, logicalHeight * windowScale, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (!window) {
		// testing for null pointer
		spdlog::error("Error creating SDL window");
		return;
	}

	// allow for toggling of fullscreen
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	
	// -1 index param means get default monitor
	// flags seperated by pipe
	// SDL_RENDERER_ACCELERATED - use GPU if available
	// SDL_RENDERER_PRESENTVSYNC - Use VSync; match frame rate with monitor refresh for smoother experience and prevents screen tearing
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		// testing for a null pointer
		spdlog::error("Error creating SDL renderer");
		return;
	}

	// initialize the camera view with the whole screen area
	camera.x = 0;
	camera.y = 0;
	camera.w = logicalWidth;
	camera.h = logicalHeight;

	SDL_SetWindowFullscreen(window, 0);

	// configure lua
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string);
	spdlog::info("lua state created");

	gameIsRunning = true;
}

void Game::Setup() {
	// create the bindings between c++ and lua
	scriptSystem->CreateLuaBindings(lua);

	// load first level
	MapLoader loader;
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
	std::string mapName = "init";
	loader.LoadMap(lua, registry, assetStore, renderer, mapName);

	entt::entity textBox = registry.create();
	registry.emplace<TextComponent>(textBox,"Hello World!/Love You!", 18, 8, 32, 160, true);
}

void Game::Run() {
	Setup();
	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
	}
}

void Game::ProcessInput() {
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {

		// handle core sdl events
		switch (sdlEvent.type) {
		case SDL_QUIT:  // if user tries to close the window using the x button
			gameIsRunning = false;
			break;
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			// exit the game if user presses escape key
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
				gameIsRunning = false;
				break;
			}
			// toggle debug mode if user presses tilde key
			if (sdlEvent.key.keysym.sym == SDLK_BACKQUOTE) {
				spdlog::info("debug mode engaged");
				debugMode = !debugMode; // toggle
				break;
			}
			if (sdlEvent.key.keysym.sym == SDLK_x) {
				spdlog::info("action button pressed");
				textSystem.RenderTextBox(registry, renderer, camera, assetStore);
			}
			dispatcher.enqueue<KeyPressedEvent>({ sdlEvent });
			break;
		}
	}
}

void Game::Update() {
	// if we are too fast, waste some time until we reach MILLISECS_PER_FRAME
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), millisecondsPreviousFrame + MILLISECS_PER_FRAME));
	// SDL_Delay is way more efficient than the above while loop since it doesn't burn clock cycles while waiting
	// can comment out below so it runs at highest framerate possible since we're using delta time now. I won't do that though.
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame);
	if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
		SDL_Delay(timeToWait);
	}

	// the difference in ticks since the last frame converted to seconds.
	double deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame) / 1000.0;

	// store the current frame time
	millisecondsPreviousFrame = SDL_GetTicks();

	dispatcher.update(); // TODO RIGOLO - Not sure if this is the best spot to call the dispatchers update method : keep an eye on this
	AnimationSystem(registry);
	MovementSystem(registry, deltaTime);
	CameraMovementSystem(registry, camera); // TODO RIGOLO - SHOULD THIS BE HERE OR IN THE RENDERING SECTION?
}

void Game::Render() {
	// render window
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	// Invoke all systems that need to render
	RenderSystem(registry, renderer, camera, assetStore);
	
	// debugging collision detection
	if (debugMode) {
	/*	registry.GetSystem<RenderColliderSystem>().Update(renderer, camera);*/
	}
 
	//// Set logical size so our drawing uses NES-ish resolution regardless of window size
	SDL_RenderSetLogicalSize(renderer, logicalWidth, logicalHeight);
	SDL_RenderPresent(renderer);
}

void Game::Destroy() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}