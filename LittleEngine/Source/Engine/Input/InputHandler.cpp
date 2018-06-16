#include "stdafx.h"
#include "InputHandler.h"
#include "SFMLInterface/Input.h"
#include "Engine/Logger/Logger.h"

namespace Game {
	InputHandler::Observer::Observer(Delegate<const KeyState&> callback, bool consume)
		: callback(callback), consume(consume) {
	}

	InputHandler::InputHandler() : Object("InputHandler") {
		Logger::Log(*this, "InputHandler constructed");
	}

	InputHandler::~InputHandler() {
		Logger::Log(*this, "InputHandler destroyed");
	}

	Delegate<const KeyState&>::Token Game::InputHandler::Register(Delegate<const KeyState&>::Callback callback, KeyCode keyCode, bool consume) {
		Delegate<const KeyState&> newDelegate;
		Delegate<const KeyState&>::Token token = newDelegate.Register(callback);
		Observer observer(newDelegate, consume);
		
		auto iter = observers.find(keyCode);
		size_t size = 0;
		if (iter != observers.end()) {
			auto& vec = iter->second;
			vec.push_back(observer);
			size = vec.size();
		}
		else {
			std::vector<Observer> newVec;
			newVec.push_back(observer);
			observers.insert(std::pair<KeyCode, std::vector<Observer> >(keyCode, newVec));
			size = newVec.size();
		}
		Logger::Log(*this, "Registered new callback for KeyCode: " + std::to_string((int)keyCode) + " Total: " + std::to_string(size));
		return token;
	}

	void InputHandler::Cleanup(std::vector<Observer>& vec) {
		int before = vec.size();
		vec.erase(std::remove_if(vec.begin(), vec.end(),
			[](Observer& observer) {
				return !observer.callback.IsAlive();
			}
		), vec.end());
		int deleted = before - vec.size();
		if (deleted > 0) {
			Logger::Log(*this, std::to_string(deleted) + " expired Observers deleted", Logger::Severity::Debug);
		}
	}

	void InputHandler::FireInput(const Input & input) {
		std::vector<KeyState> pressed = input.GetPressed();
		for (auto& keyState : pressed) {
			auto iter = observers.find(keyState.GetKeyCode());
			if (iter != observers.end()) {
				auto& vec = iter->second;
				Cleanup(vec);
				for (auto iter = vec.rbegin(); iter != vec.rend(); ++iter) {
					auto& observer = *iter;
					observer.callback(keyState);
					if (observer.consume) {
						break;
					}
				}
			}
		}
	}
}
