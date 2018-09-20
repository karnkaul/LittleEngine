#pragma once
#include <unordered_map>
#include "Engine/Object.h"
#include "Utils/Delegate.hpp"

namespace LittleEngine {
	enum class GameEvent {
		DEBUG_ON = 100,
		DEBUG_OFF = 101,
	};
	
	using OnEvent = Utils::Action;
	
	// \brief: Central point of all GameEvent dispatching
	class EventManager : public Object {
	public:
		static EventManager& Instance();

		// Discard received token to unsubscribe
		OnEvent::Token Register(GameEvent eventType, OnEvent::Callback Callback);
		// Returns subscriber count
		int Notify(GameEvent eventType);
	
	private:
		using OnEventPtr = std::shared_ptr<OnEvent>;
		using Subscribers = std::unordered_map<GameEvent, OnEventPtr>;
		
		Subscribers subscribers;

		EventManager();
	};
}
