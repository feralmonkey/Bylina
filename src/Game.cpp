#include "Game.h"
#include "MapLoader.h"
#include "components/PlayerComponent.h"
#include "components/SpriteComponent.h"
#include "components/StatsComponent.h"

#include "events/KeyPressedEvent.h"
#include "events/KeyUpEvent.h"
#include "systems/AnimationSystem.h"
#include "systems/CameraMovementSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/KeyboardControlSystem.h"
#include "systems/MenuSystem.h"
#include "systems/MovementSystem.h"
#include "systems/NPCSystem.h"
#include "systems/RenderColliderSystem.h"
#include "systems/RenderSystem.h"
#include "systems/RenderTextSystem.h"
#include "systems/ScriptSystem.h"

Game::Game() :
	gameIsRunning(false),
	dispatcher(),
	registry()
{
	spdlog::info("Game constructor called!");
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
	
	config.windowScale = 3;
	config.logicalWidth = 256;
	config.logicalHeight = 240;

	// create the game window
	window = SDL_CreateWindow(
		"Bylina", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		config.logicalWidth * config.windowScale, config.logicalHeight * config.windowScale, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (!window) {
		// testing for a null pointer
		spdlog::error("Error creating SDL window");
		return;
	}

	// allow for toggling of fullscreen
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	
	// -1 index param means get default monitor
	// flags seperated by pipe
	// SDL_RENDERER_ACCELERATED - use GPU if available
	// SDL_RENDERER_PRESENTVSYNC - Use VSync; match frame rate with monitor refresh for a smoother experience and prevents screen tearing
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		// testing for a null pointer
		spdlog::error("Error creating SDL renderer");
		return;
	}

	// initialize the camera view with the whole screen area
	camera.x = 0;
	camera.y = 0;
	camera.w = config.logicalWidth;
	camera.h = config.logicalHeight;

	SDL_SetWindowFullscreen(window, 0);

	// configure lua
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string);
	spdlog::info("lua state created");

	assetStore = std::make_unique<AssetStore>();

	// Now systems can be created safely
	textSystem = std::make_unique<RenderTextSystem>(registry, dispatcher, camera,  inputStack);
	keyboardSystem = std::make_unique<KeyboardControlSystem>(registry, dispatcher, inputStack, *textSystem);
	movementSystem = std::make_unique<MovementSystem>(registry, dispatcher);
	collisionSystem = std::make_unique<CollisionSystem>(registry, dispatcher, 8);
	collisionResolutionSystem = std::make_unique<CollisionResolutionSystem>(registry, dispatcher);
	menuSystem = std::make_unique<MenuSystem>();
	npcSystem = std::make_unique<NPCSystem>(registry, dispatcher, *textSystem);
	scriptSystem = std::make_unique<ScriptSystem>();

	gameIsRunning = true;
}

void Game::Setup() {
	// create the bindings between c++ and lua
	scriptSystem->CreateLuaBindings(lua);

	// load first level
	MapLoader loader;
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
	//const std::string mapName = "init";
	const std::string mapName = "overworld";
	loader.LoadMap(lua, registry, assetStore, renderer, mapName, config);

	// not actually using this here am I...
	collisionSystem->SetMapDimensions(config.mapWidth, config.mapHeight);
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
		case SDL_QUIT:  // if the user tries to close the window using the x button...
			gameIsRunning = false; 
			break;
		case SDL_KEYDOWN:
			// exit the game if the user presses the escape key
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
				gameIsRunning = false;
				break;
			}
			// toggle debug mode if user presses tilde key
				if (sdlEvent.key.keysym.sym == SDLK_BACKQUOTE) {
					spdlog::info("debug mode engaged");
					config.debugMode = !config.debugMode;

					// Show stats in debug mode
					if (config.debugMode) {
						auto view = registry.view<PlayerComponent, StatsComponent>();
							const auto& player = view.front();
							if (player == entt::null) { break; }

							const auto& stats = view.get<StatsComponent>(player);
							const std::string statsMsg = "HP:" + std::to_string(stats.currentHP) + " " +
												   "MP:" + std::to_string(stats.currentMP) + " " +
												   "G:" + std::to_string(stats.gold);
						textSystem->TextBox(statsMsg, 20, 3, 8, 8);
					} else {
						textSystem->ClearText();
					}
					break;
				}
			dispatcher.enqueue<KeyPressedEvent>({ sdlEvent });
			break;
		case SDL_KEYUP:
			dispatcher.enqueue<KeyUpEvent>({ sdlEvent });
			break;
		default:
			break;
		}
	}
}

void Game::Update() {
	// frame timing
	uint timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame);
	if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
		SDL_Delay(timeToWait);
	}
	const auto deltaTime = static_cast<double>((SDL_GetTicks() - millisecondsPreviousFrame) / 1000.0);
	millisecondsPreviousFrame = SDL_GetTicks();

	// 0. Move NPCs
	npcSystem->Update(deltaTime);

	// 1. deliver input events so systems can react
	dispatcher.update();

	// 1.5. Update keyboard system (for continuous movement)
	if (keyboardSystem) keyboardSystem->Update(deltaTime);

	// 2. movement (positions change)
	if (movementSystem) movementSystem->Update(deltaTime);

	// 3. collision detection (enqueue collision events)
	if (collisionSystem) collisionSystem->Update();

	// 4. deliver collision events so resolution runs *this frame*
	dispatcher.update();

	// 5. other systems
	AnimationSystem(registry);           // updates animations
	CameraMovementSystem(registry, camera, config); // camera follows player
}

void Game::Render() {
	// render window
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	// Invoke all systems that need to render
	RenderSystem(registry, renderer, camera, assetStore);
	
	// debugging collision detection
	if (config.debugMode) {
		RenderColliderSystem(registry, renderer, camera);
	}
 
	//// Set logical size so our drawing uses NES-ish resolution regardless of window size
	SDL_RenderSetLogicalSize(renderer, config.logicalWidth, config.logicalHeight);
	SDL_RenderPresent(renderer);
}

void Game::Destroy() const {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
