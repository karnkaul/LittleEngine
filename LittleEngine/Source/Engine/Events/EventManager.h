#pragma once
#include <unordered_map>
#include "Engine/Object.h"
#include "Delegate.hpp"

namespace LittleEngine {
	enum class GameEvent {
		DEBUG_SHOW_COLLIDERS = 100,
		DEBUG_HIDE_COLLIDERS = 101,
	};
	
	using OnEvent = GameUtils::Delegate<>;
	
	// \brief: Central point of all GameEvent dispatching
	class EventManager : public Object {
	public:
		static EventManager& Instance();

		// Discard received token to unsubscribe
		OnEvent::Token Register(GameEvent eventType, OnEvent::Callback Callback);
		// Returns subscriber count
		int Notify(GameEvent eventType);
	
	private:
		using Subscribers = std::unordered_map<GameEvent, OnEvent>;
		
		Subscribers subscribers;

		EventManager();
	};
}
