#include "Game.h"
#include "components/TransformComponent.h"
#include "components/RigidBodyComponent.h"
#include "components/SpriteComponent.h"
#include "components/AnimationComponent.h"
#include "components/PlayerComponent.h"
#include "components/KeyboardControlComponent.h" // TODO RIGOLO - not sure that i need this
#include "components/TextComponent.h"
#include "systems/RenderSystem.h"
#include "systems/AnimationSystem.h"
#include "events/KeyPressedEvent.h"
#include "systems/KeyboardControlSystem.h"
#include "systems/MovementSystem.h"
#include "systems/RenderTextSystem.h"

// initialize static member variables
int Game::logicalWidth;
int Game::logicalHeight;
int Game::windowScale;
int Game::mapWidth;
int Game::mapHeight;

Game::Game() {
	spdlog::info("Game constructor called!");
	Game::gameIsRunning = false;
	debugMode = false;

	assetStore = std::make_unique<AssetStore>();
	eventBus = std::make_unique<EventBus>();
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
	camera.w = logicalWidth;  // might have to factor in the multiplier here TODO RIGOLO
	camera.h = logicalHeight;

	SDL_SetWindowFullscreen(window, 0);
	gameIsRunning = true;
}

void Game::Setup() {
	// LOAD LEVEL
	// Adding assets to the asset store
	assetStore->AddTexture(renderer, "hero", "./assets/images/heroes.png");
	assetStore->AddTexture(renderer, "character-tiles", "./assets/images/character_tiles.png");
	assetStore->AddTexture(renderer, "outdoor-tiles", "./assets/images/outdoor_tiles.png");
	assetStore->AddTexture(renderer, "town-tiles", "./assets/images/town_tiles.png");	assetStore->AddTexture(renderer, "tilemap-image", "./assets/images/outdoor_tiles.png");

	// load the tilemap
	int tileSize = 8;
	double tileScale = 1.0;
	int mapNumCols = 25;
	int mapNumRows = 20;

	std::fstream mapFile;
	mapFile.open("./assets/tilemaps/outdoor-sample1.map");

	for (int y = 0; y < mapNumRows; y++) {
		for (int x = 0; x < mapNumCols; x++) {
			char ch;
			
			// get the first character and convert hex character to integer
			mapFile.get(ch);
			int valueY = std::stoi(std::string(1, ch), nullptr, 16);
			int srcRectY = valueY * tileSize;

			// get second character and convert
			mapFile.get(ch);
			int valueX = std::stoi(std::string(1, ch), nullptr, 16);
			int srcRectX = valueX * tileSize;

			// ignore commas
			mapFile.ignore();

			entt::entity tile = registry.create();
			/*tile.Group("tiles");*/
			registry.emplace<TransformComponent>(tile, glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
			registry.emplace<SpriteComponent>(tile, "outdoor-tiles", tileSize, tileSize, 0, false, srcRectX, srcRectY);
		}
	}
	mapFile.close();
	mapWidth = mapNumCols * tileSize * tileScale;
	mapHeight = mapNumRows * tileSize * tileScale;

	entt::entity hero = registry.create();
	registry.emplace<TransformComponent>(hero, glm::vec2(32.0, 32.0), glm::vec2(1.0, 1.0), 0.0);
	registry.emplace<RigidBodyComponent>(hero);
	registry.emplace<SpriteComponent>(hero, "hero", 16, 16, 1);
	registry.emplace<AnimationComponent>(hero, 2, 4, true);
	registry.emplace<PlayerComponent>(hero, true);

	entt::entity textBox = registry.create();
	registry.emplace<TextComponent>(textBox,"Hello World!", glm::vec2(8,8), 8, 6, false); // use default parameters for now

	// create the bindings between c++ and lua
	//registry.get<ScriptSystem>().CreateLuaBindings(lua); //GetSystem<ScriptSystem>().CreateLuaBindings(lua);

	// load first level
	/* todo rigolo change to entt logic
	LevelLoader loader;
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
	loader.LoadLevel(lua, registry, assetStore, renderer, 2);
	*/ 
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
				RenderTextBox(registry, renderer, camera, assetStore);
			}
			if (sdlEvent.key.keysym.sym == SDLK_z) {
				spdlog::info("cancel button pressed");
				ClearTextBox(registry);
			}
			KeyboardControlSystem(sdlEvent, registry);
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

	// reset all event handlers for current frame
	eventBus->Reset();

	AnimationSystem(registry);
	MovementSystem(registry, deltaTime);

	/* -- todo rigolo change update and subscription systems
	// perform the subscription of events of all systems
	registry.GetSystem<MovementSystem>().SubscribeToEvents(eventBus);

	// update systems
	registry.GetSystem<MovementSystem>().Update(deltaTime);
	registry.GetSystem<CollisionSystem>().Update(eventBus);
	registry.GetSystem<CameraMovementSystem>().Update(camera);
	registry.GetSystem<ScriptSystem>().Update(deltaTime, SDL_GetTicks());
	*/
}

void Game::Render() {
	// render window
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	// Invoke all systems that need to render
	RenderSystem(registry, renderer, camera, assetStore);
	/* todo rigolo modify render update systems
	registry.GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);

	// debugging collision detection
	if (debugMode) {
		registry.GetSystem<RenderColliderSystem>().Update(renderer, camera);
	}
	*/ 
	//// Set logical size so our drawing uses NES-ish resolution regardless of window size
	SDL_RenderSetLogicalSize(renderer, logicalWidth, logicalHeight);
	SDL_RenderPresent(renderer);
}

void Game::Destroy() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}