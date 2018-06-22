#include "stdafx.h"
#include "InputHandler.h"
#include "Engine/Logger/Logger.h"
#include "SFMLInterface/Input.h"

namespace Game {
	InputHandler::InputObserver::InputObserver(OnInput&& callback, bool consume)
		: callback(std::move(callback)), consume(consume) {
	}

	InputHandler::InputObserver & InputHandler::InputObserver::operator=(InputObserver && move) {
		callback = std::move(move.callback);
		consume = move.consume;
		return *this;
	}

	InputHandler::InputHandler() : Object("InputHandler") {
		SetupInputBindings();
		Logger::Log(*this, "InputHandler constructed");
	}

	InputHandler::~InputHandler() {
		Logger::Log(*this, "InputHandler destroyed");
	}

	bool InputHandler::IsKeyPressed(GameInput keyCode) const {
		auto iter = std::find(storedState.begin(), storedState.end(), keyCode);
		return iter != storedState.end();
	}

	OnInput::Token Game::InputHandler::Register(OnInput::Callback callback, GameInput keyCode, bool consume) {
		OnInput newDelegate;
		OnInput::Token token = newDelegate.Register(callback);
		
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
			observers.insert(std::pair<GameInput, std::vector<InputObserver> >(keyCode, std::move(newVec)));
		}
		Logger::Log(*this, "Registered new callback for KeyCode: " + std::to_string((int)keyCode) + " Total: " + std::to_string(size));
		return token;
	}

	void InputHandler::SetupInputBindings() {
		gamepad.Bind(KeyCode::Up, GameInput::Up);
		gamepad.Bind(KeyCode::W, GameInput::Up);
		gamepad.Bind(KeyCode::Down, GameInput::Down);
		gamepad.Bind(KeyCode::S, GameInput::Down);
		gamepad.Bind(KeyCode::Left, GameInput::Left);
		gamepad.Bind(KeyCode::A, GameInput::Left);
		gamepad.Bind(KeyCode::Right, GameInput::Right);
		gamepad.Bind(KeyCode::D, GameInput::Right);
		gamepad.Bind(KeyCode::Enter, GameInput::Enter);
		gamepad.Bind(KeyCode::Escape, GameInput::Return);
		gamepad.Bind(KeyCode::Tab, GameInput::Select);

		gamepad.Bind(KeyCode::Space, GameInput::X);
		gamepad.Bind(KeyCode::E, GameInput::Y);
		gamepad.Bind(KeyCode::Control, GameInput::LB);
		gamepad.Bind(KeyCode::Shift, GameInput::RB);
	}

	void InputHandler::CaptureState(const std::vector<KeyState>& pressedKeys) {
		storedState.clear();
		for (const auto& key : pressedKeys) {
			if (key.GetKeyCode() != KeyCode::Invalid) {
				storedState.push_back(gamepad.ToGameInput(key));
			}
		}
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

	void InputHandler::FireInput() {
		for (auto& key : storedState) {
			auto iter = observers.find(key);
			if (iter != observers.end()) {
				auto& vec = iter->second;
				Cleanup(vec);
				for (auto iter = vec.rbegin(); iter != vec.rend(); ++iter) {
					auto& observer = *iter;
					observer.callback();
					if (observer.consume) {
						break;
					}
				}
			}
		}
	}
}
