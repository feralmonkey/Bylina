#pragma once
#include "../events/EventBus.h"
#include "Event.h"
#include <SDL.h>
 
class KeyPressedEvent : public Event {
	public:
        SDL_Keycode symbol;
        KeyPressedEvent(SDL_Keycode symbol) : symbol(symbol) {}
};