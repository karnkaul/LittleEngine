#include "stdafx.h"
#include <string>
#include "InputHandler.h"
#include "Engine/Logger/Logger.h"
#include "SFMLInterface/Input.h"

namespace Game {
	InputHandler::Observer::Observer(std::weak_ptr<Callback> callback, KeyCode keyCode, bool consume) 
		: callback(callback), keyCode(keyCode), consume(consume) {
	}

	InputHandler::InputHandler() : Object("InputHandler") {
		Logger::Log(*this, "InputHandler constructed");
	}

	InputHandler::~InputHandler() {
		Logger::Log(*this, "InputHandler destroyed");
	}

	InputHandler::Token InputHandler::Register(Callback callback, KeyCode keyCode, bool consume) {
		auto iter = observers.find(keyCode);
		auto ptr = std::make_shared<Callback>(callback);
		int size = 0;
		if (iter != observers.end()) {
			auto& vec = iter->second;
			vec.push_back(Observer(ptr, keyCode, consume));
			size = vec.size();
		}
		else {
			std::vector<Observer> vec;
			vec.push_back(Observer(ptr, keyCode, consume));
			observers.insert(std::pair<KeyCode, std::vector<Observer> >(keyCode, std::move(vec)));
			size = 1;
		}
		Logger::Log(*this, "Registered new callback for KeyCode " + std::to_string((int)keyCode) + " Total: " + std::to_string(size));
		return ptr;
	}

	//void InputHandler::Unregister(Callback callback, KeyCode keyCode) {
	//	auto iter = observers.find(keyCode);
	//	if (iter != observers.end()) {
	//		auto vec = iter->second;
	//		int before = vec.size();
	//		vec.erase(std::remove_if(vec.begin(), vec.end(), 
	//			[callback, keyCode](Observer ob) { 
	//				return &ob.callback == &callback && keyCode == ob.keyCode;
	//			}
	//		), vec.end());
	//		if (vec.size() - 1 != before) {
	//			Logger::Log(*this, "Error in unregistering callback for KeyCode " + std::to_string((int)keyCode) + " Before: " + std::to_string(before) + " After: " + std::to_string(vec.size()), Logger::Severity::Warning);
	//		}
	//		else {
	//			Logger::Log(*this, "Unregistered callback for KeyCode " + std::to_string((int)keyCode) + " Total: " + std::to_string(vec.size()));
	//		}
	//	}
	//}

	void InputHandler::FireInput(const Input & input) {
		std::vector<KeyState> pressed = input.GetPressed();
		for (const auto& keyState : pressed) {
			auto iter = observers.find(keyState.GetKeyCode());
			if (iter != observers.end()) {
				auto& vec = iter->second;
				int before = vec.size();
				vec.erase(std::remove_if(vec.begin(), vec.end(),
					[](Observer observer) {
						return observer.callback.lock() == nullptr;
					}
				), vec.end());
				int after = vec.size();
				for(auto iter = vec.rbegin(); iter != vec.rend(); ++iter) {
					std::shared_ptr<Callback> callback = iter->callback.lock();
					(*callback)(keyState);
					if (iter->consume) {
						break;
					}
				}
			}
		}
	}
}
