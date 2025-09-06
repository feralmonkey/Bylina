#pragma once
#include "spdlog.h"
#include "../events/EventBus.h"
#include "Event.h"
#include <SDL.h>
 
class KeyPressedEvent : public Event {
	public:
        SDL_Keycode symbol;
        KeyPressedEvent(SDL_Keycode symbol) : symbol(symbol) { spdlog::info("Key Press Detected"); }
};