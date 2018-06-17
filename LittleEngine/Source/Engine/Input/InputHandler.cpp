#include "stdafx.h"
#include "InputHandler.h"
#include "Engine/Logger/Logger.h"
#include "SFMLInterface/Input.h"

namespace Game {
	InputHandler::InputObserver::InputObserver(Delegate<const KeyState&>&& callback, bool consume)
		: callback(std::move(callback)), consume(consume) {
	}

	InputHandler::InputObserver & InputHandler::InputObserver::operator=(InputObserver && move) {
		callback = std::move(move.callback);
		consume = move.consume;
		return *this;
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
		
		auto iter = observers.find(keyCode);
		size_t size = 0;
		if (iter != observers.end()) {
			auto& vec = iter->second;
			vec.emplace_back(std::move(newDelegate), consume);
			size = vec.size();
		}
		else {
			std::vector<InputObserver> newVec {
				InputObserver(std::move(newDelegate), consume)
			};
			size = newVec.size();
			observers.insert(std::pair<KeyCode, std::vector<InputObserver> >(keyCode, std::move(newVec)));
		}
		Logger::Log(*this, "Registered new callback for KeyCode: " + std::to_string((int)keyCode) + " Total: " + std::to_string(size));
		return token;
	}

	void InputHandler::Cleanup(std::vector<InputObserver>& vec) {
		int before = vec.size();
		vec.erase(std::remove_if(vec.begin(), vec.end(),
			[](InputObserver& observer) {
				return !observer.callback.IsAlive();
			}
		), vec.end());
		int deleted = before - vec.size();
		if (deleted > 0) {
			Logger::Log(*this, std::to_string(deleted) + " expired Observers deleted", Logger::Severity::Debug);
		}
	}

	void InputHandler::FireInput(const std::vector<KeyState>& pressedKeys) {
		for (auto& keyState : pressedKeys) {
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
