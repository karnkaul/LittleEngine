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
		auto iter = std::find(currentSnapshot.begin(), currentSnapshot.end(), keyCode);
		return iter != currentSnapshot.end();
	}

	OnInput::Token InputHandler::OnPressed(GameInput input, OnInput::Callback callback, bool consume) {
		Logger::Log(*this, "Registered OnPressed callback for GameInput: " + std::to_string((int)input) + " Total: " + std::to_string(onPressedObservers.size()) + " [consume: " + std::to_string(consume) + "]");
		return Register(onPressedObservers, callback, input, consume);
	}

	OnInput::Token Game::InputHandler::OnHeld(GameInput input, OnInput::Callback callback, bool consume) {
		Logger::Log(*this, "Registered OnHeld callback for GameInput: " + std::to_string((int)input) + " Total: " + std::to_string(onHeldObservers.size()) + " [consume: " + std::to_string(consume) + "]");
		return Register(onHeldObservers, callback, input, consume);
	}

	OnInput::Token InputHandler::OnReleased(GameInput input, OnInput::Callback callback, bool consume) {
		Logger::Log(*this, "Registered OnReleased callback for GameInput: " + std::to_string((int)input) + " Total: " + std::to_string(onReleasedObservers.size()) + " [consume: " + std::to_string(consume) + "]");
		return Register(onReleasedObservers, callback, input, consume);
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
		previousSnapshot = currentSnapshot;
		currentSnapshot.clear();
		for (const auto& key : pressedKeys) {
			GameInput input = gamepad.ToGameInput(key);
			if (input != GameInput::Invalid) {
				auto duplicate = std::find(currentSnapshot.begin(), currentSnapshot.end(), input);
				if (duplicate == currentSnapshot.end()) {
					currentSnapshot.push_back(input);
				}
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

	OnInput::Token InputHandler::Register(std::unordered_map<GameInput, std::vector<InputObserver>>& map, OnInput::Callback callback, GameInput keyCode, bool consume) {
		OnInput newDelegate;
		OnInput::Token token = newDelegate.Register(callback);
		auto iter = map.find(keyCode);
		size_t size = 0;
		if (iter != map.end()) {
			auto& vec = iter->second;
			vec.emplace_back(std::move(newDelegate), consume);
			size = vec.size();
		}
		else {
			std::vector<InputObserver> newVec{
				InputObserver(std::move(newDelegate), consume)
			};
			size = newVec.size();
			map.insert(std::pair<GameInput, std::vector<InputObserver> >(keyCode, std::move(newVec)));
		}
		return token;
	}

	void InputHandler::FireCallbacks(const std::vector<GameInput>& inputs, std::unordered_map<GameInput, std::vector<InputObserver>>& map) {
		for (auto& key : inputs) {
			auto iter = map.find(key);
			if (iter != map.end()) {
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

	void InputHandler::FireInput() {
		// Build OnPressed, OnHeld and OnReleased
		std::vector<GameInput> onPressed, onHeld, onReleased;
		for (auto input : currentSnapshot) {
			auto search = std::find(previousSnapshot.begin(), previousSnapshot.end(), input);
			if (search != previousSnapshot.end()) {
				onHeld.push_back(input);
				previousSnapshot.erase(search);
			}
			else {
				onPressed.push_back(input);
			}
		}
		for (const auto& input : previousSnapshot) {
			onReleased.push_back(input);
		}

		// Dispatch all callbacks
		FireCallbacks(onPressed, onPressedObservers);
		FireCallbacks(onHeld, onHeldObservers);
		FireCallbacks(onReleased, onReleasedObservers);
	}
}
