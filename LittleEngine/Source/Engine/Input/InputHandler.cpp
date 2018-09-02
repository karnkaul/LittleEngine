#include "stdafx.h"
#include "InputHandler.h"
#include "Engine/Logger/Logger.h"
#include "SFMLInterface/Input.h"
#include "Utils/Utils.h"

namespace LittleEngine {
	InputHandler::InputObserver::InputObserver(OnInput&& callback, bool consume, const OnKey& type)
		: callback(std::move(callback)), consume(consume), type(type) {
	}

	InputHandler::InputObserver & InputHandler::InputObserver::operator=(InputObserver && move) {
		callback = std::move(move.callback);
		consume = move.consume;
		type = move.type;
		return *this;
	}

	InputHandler::InputHandler() : Object("InputHandler") {
		/* Setup Input Bindings */ {
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
		Logger::Log(*this, "InputHandler constructed");
	}

	InputHandler::~InputHandler() {
		Logger::Log(*this, "InputHandler destroyed");
	}

	bool InputHandler::IsKeyPressed(const GameInput& keyCode) const {
		auto iter = Utils::VectorSearch(currentSnapshot, keyCode);
		return iter != currentSnapshot.end();
	}

	OnInput::Token InputHandler::Register(const GameInput& input, OnInput::Callback callback, const OnKey& type, bool consume) {
		OnInput newDelegate;
		OnInput::Token token = newDelegate.Register(callback);
		auto iter = inputObservers.find(input);
		size_t size = 0;
		if (iter != inputObservers.end()) {
			auto& vec = iter->second;
			vec.emplace_back(std::move(newDelegate), consume, type);
			size = vec.size();
		}
		else {
			std::vector<InputObserver> newVec {
				InputObserver(std::move(newDelegate), consume, type)
			};
			size = newVec.size();
			inputObservers.insert(std::pair<GameInput, std::vector<InputObserver>>(input, std::move(newVec)));
		}
		return token;
	}

	void InputHandler::CaptureState(const std::vector<KeyState>& pressedKeys) {
		previousSnapshot = currentSnapshot;
		currentSnapshot.clear();
		for (const auto& key : pressedKeys) {
			GameInput input = gamepad.ToGameInput(key);
			if (input != GameInput::Invalid) {
				auto duplicate = Utils::VectorSearch(currentSnapshot, input);
				if (duplicate == currentSnapshot.end()) {
					currentSnapshot.push_back(input);
				}
			}
		}
	}

	void InputHandler::Cleanup(std::vector<InputObserver>& vec) {
		int before = vec.size();
		Utils::CleanVector<InputObserver>(vec, [](InputObserver& observer) { return !observer.callback.IsAlive(); });
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
			vec.emplace_back(std::move(newDelegate), consume, OnKey::Held);
			size = vec.size();
		}
		else {
			std::vector<InputObserver> newVec{
				InputObserver(std::move(newDelegate), consume, OnKey::Held)
			};
			size = newVec.size();
			map.insert(std::pair<GameInput, std::vector<InputObserver>>(keyCode, std::move(newVec)));
		}
		return token;
	}

	void InputHandler::FireCallbacks(const std::vector<GameInput>& inputs, std::unordered_map<GameInput, std::vector<InputObserver>>& map, OnKey type) {
		for (const auto& input : inputs) {
			auto iter = map.find(input);
			if (iter != map.end()) {
				auto& vec = iter->second;
				Cleanup(vec);
				for (auto iter = vec.rbegin(); iter != vec.rend(); ++iter) {
					auto& observer = *iter;
					if (observer.type == type) {
						observer.callback();
					}
					if (observer.consume) {
						break;
					}
				}
			}
		}
	}

	void InputHandler::FireInput() {
		std::vector<GameInput> onPressed, onHeld;
		
		// Build OnPressed and OnHeld vectors
		for (auto input : currentSnapshot) {
			auto search = Utils::VectorSearch(previousSnapshot, input);
			if (search != previousSnapshot.end()) {
				onHeld.push_back(input);
				previousSnapshot.erase(search);
			}
			else {
				onPressed.push_back(input);
			}
		}

		// Fire callbacks
		FireCallbacks(onPressed, inputObservers, OnKey::Pressed);
		FireCallbacks(onHeld, inputObservers, OnKey::Held);
		FireCallbacks(previousSnapshot, inputObservers, OnKey::Released);
	}
}
