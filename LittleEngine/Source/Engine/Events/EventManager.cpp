#include "le_stdafx.h"
#include "EventManager.h"
#include "Engine/Logger/Logger.h"

namespace LittleEngine {
	EventManager& EventManager::Instance() {
		static EventManager eventManager;
		return eventManager;
	}

	OnEvent::Token EventManager::Register(GameEvent eventType, OnEvent::Callback Callback) {
		auto search = subscribers.find(eventType);
		if (search != subscribers.end()) {
			return search->second->Register(Callback);
		}
		
		std::shared_ptr<OnEvent> _event = std::make_shared<OnEvent>();
		subscribers.insert(std::pair<GameEvent, OnEventPtr>(eventType, _event));
		return _event->Register(Callback);
	}

	int EventManager::Notify(GameEvent eventType) {
		auto search = subscribers.find(eventType);
		if (search != subscribers.end()) {
			int count = (*search->second)();
			Logger::Log(*this, "[GameEvent " + std::to_string(static_cast<int>(eventType)) + "] fired [" + std::to_string(count) + " observers]", Logger::Severity::Debug);
		}
		return 0;
	}
	
	EventManager::EventManager() : Object("EventManager") {
	}
}
