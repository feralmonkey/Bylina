#pragma once

#include <spdlog/spdlog.h>
#include "Event.h"
#include <map>
#include <typeindex> // I don't know this one

class IEventCallback {
	// base class for EventCallback
	private:
		virtual void Call(Event& e) = 0;

	public:
		virtual ~IEventCallback() = default;

		void Execute(Event& e) {
			Call(e);
		}
};

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback {
	// here is the callback function pointer
	// that needs to be invoked
	private:
		typedef void (TOwner::*CallbackFunction)(TEvent&);

		TOwner* ownerInstance;
		CallbackFunction callbackFunction;

		virtual void Call(Event& e) override {
			std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
		}

	public:
		EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction) {
			this->ownerInstance = ownerInstance;
			this->callbackFunction = callbackFunction;
		}

		virtual ~EventCallback() override = default;
};

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus {
	private:
		std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

	public:
		EventBus() {
			spdlog::info("EventBus Constructor Called!");
		}
		~EventBus() {
			spdlog::info("EventBus Destructor Called!");
		}

		// clears the subscriber list
		void Reset() {
			subscribers.clear();
		}

		////////////////////////////////////////////////////////////////////////////
		// subscribe to an event type <T>
		// a listener subscribes to an event
		// example: eventBus->SubscribeToEvent<CollisionEvent>(this, &Game::onCollision);
		////////////////////////////////////////////////////////////////////////////a
		template <typename TEvent, typename TOwner>
		void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::*callbackFunction)(TEvent&)) {
			// avoid a null pointer
			if (!subscribers[typeid(TEvent)].get()) {
				subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
			}
			auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
			subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
		}

		////////////////////////////////////////////////////////////////////////////
		// emit an event type <T>
		// as soon as something emits an event...
		// ...we execute listner callbacks
		// example: eventBus->EmitEvent<CollisionEvent>(player, enemy);
		////////////////////////////////////////////////////////////////////////////
		template <typename TEvent, typename ...TArgs>
		void EmitEvent(TArgs&& ...args) {
			auto handlers = subscribers[typeid(TEvent)].get();
			if (!handlers) {
				spdlog::info("No handlers");
				return;
			}

			for (auto it = handlers->begin(); it != handlers->end(); it++) {
				auto handler = it->get();
				TEvent event(std::forward<TArgs>(args)...); 
				handler->Execute(event);
			}
		}
};
