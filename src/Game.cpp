#include "Game.h"

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

	windowWidth = displayMode.w;
	windowHeight = displayMode.h;

	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		0 //SDL_WINDOW_BORDERLESS
	);
	if (!window) {
		// testing for null pointer
		spdlog::error("Error creating SDL window");
		return;
	}

	// -1 index param means get default monitor
	// flags seperated by pipe
	// SDL_RENDERER_ACCELERATED - use GPU if available
	// SDL_RENDERER_PRESENTVSYNC - Use VSync; match frame rate with monitor refresh for smoother experience and prevents screen tearing
	//renderer = SDL_CreateRenderer(window, -1, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		// testing for a null pointer
		spdlog::error("Error creating SDL renderer");
		return;
	}

	// change video mode to "real" full screen	
	// initialize the camera view with the whole screen area
	camera.x = 0;
	camera.y = 0;
	camera.w = windowWidth;
	camera.h = windowHeight;

	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	SDL_SetWindowFullscreen(window, 0);
	gameIsRunning = true;
}

void Game::Setup() {
	// add the systems that need to be processed in our game
	// todo rigolo - many of these aren't going to be needed
	//registry.emplace<MovementSystem>();
	//registry.emplace<RenderSystem>();
	//registry.emplace//<AnimationSystem>();
	//registry.emplace<CollisionSystem>();
	//registry.emplace<RenderColliderSystem>();
	//registry.emplace<ScriptSystem>();
	//registry.emplace<KeyboardControlSystem>();
	//registry.emplace<CameraMovementSystem>();
	//registry.emplace<RenderTextSystem>();
	//registry.emplace<DamageSystem>();
	//registry.emplace<ProjectileEmitterSystem>();
	//registry.emplace<ProjectileLifecycleSystem>();		
	//registry.emplace<RenderHealthBarSystem>();
	//registry.emplace<RenderGUISystem>();

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
		case SDL_KEYDOWN:
			// exit the game if user presses escape key
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
				gameIsRunning = false;
			}
			// toggle debug mode if user presses tilde key
			if (sdlEvent.key.keysym.sym == SDLK_BACKQUOTE) {
				debugMode = !debugMode; // toggle
			}
			//eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym); -- todo rigolo replace with something else
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

	/* -- todo rigolo chnage update and subscription systems
	// perform the subscription of events of all systems
	registry.GetSystem<MovementSystem>().SubscribeToEvents(eventBus);
	registry.GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);

	// update the registry to process items waiting in creation / deletion queue
	registry.Update();

	// update systems
	registry.GetSystem<MovementSystem>().Update(deltaTime);
	registry.GetSystem<AnimationSystem>().Update();
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
	/* todo rigolo modify render update systems
	registry.GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
	registry.GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);

	if (debugMode) {
		registry.GetSystem<RenderColliderSystem>().Update(renderer, camera);
	}
	*/

	SDL_RenderPresent(renderer);
}