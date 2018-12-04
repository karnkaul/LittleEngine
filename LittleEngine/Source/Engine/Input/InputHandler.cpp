#include "le_stdafx.h"
#include "InputHandler.h"
#include "Engine/Logger/Logger.h"
#include "SFMLInterface/Input.h"
#include "Utils.h"
#include "../Console/DebugConsole.h"

namespace LittleEngine {
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

			gamepad.Bind(KeyCode::F12, GameInput::Debug0);
		}

		generalObservers = {
			{ OnKey::Pressed, InputMap() },
			{ OnKey::Held, InputMap() },
			{ OnKey::Released, InputMap() }
		};

		consumingObservers = {
			{ OnKey::Pressed, InputVecMap() },
			{ OnKey::Held, InputVecMap() },
			{ OnKey::Released, InputVecMap() }
		};
		Logger::Log(*this, "InputHandler constructed");
	}

	InputHandler::~InputHandler() {
		Logger::Log(*this, "InputHandler destroyed");
	}

	bool InputHandler::IsKeyPressed(const GameInput& keyCode) const {
		auto iter = GameUtils::VectorSearch(currentSnapshot, keyCode);
		return iter != currentSnapshot.end();
	}

	OnInput::Token InputHandler::Register(const GameInput& input, OnInput::Callback callback, const OnKey& type, bool bConsume) {
		// Consumers get a Delegate<> each, and go into a vector in consumingObservers
		if (bConsume) {
			// Prepare new delegate and token
			OnInput newDelegate;
			OnInput::Token token = newDelegate.Register(callback);
			InputVecMap& toModify = consumingObservers[type];
			auto iter = toModify.find(input);
			// Vector exists, add new delegate
			if (iter != toModify.end()) {
				iter->second.emplace_back(std::move(newDelegate));
			}
			// Create vector and add to map
			else {
				std::vector<OnInput> vec = { std::move(newDelegate) };
				toModify.emplace(input, std::move(vec));
			}
			return token;
		}
		// General observers are registered to the same delegate, go into generalObservers
		else {
			InputMap& toModify = generalObservers[type];
			auto iter = toModify.find(input);
			// Delegate doesn't exist, create new and add to map
			if (iter == toModify.end()) {
				toModify.emplace(input, OnInput());
				return toModify[input].Register(callback);
			}
			// Delegate exists, register directly
			return iter->second.Register(callback);
		}
	}

	void InputHandler::ProcessInput(const Input & sfmlInput) {
		rawTextInput = sfmlInput.GetRawSFMLInput();
#if ENABLED(DEBUG_CONSOLE)
		if (rawTextInput.Contains('`')) DebugConsole::Activate(!DebugConsole::IsActive());
		if (DebugConsole::IsActive()) DebugConsole::UpdateInput(rawTextInput);
		else
#endif
		{
			previousSnapshot = currentSnapshot;
			currentSnapshot.clear();
			const std::vector<KeyState> keyStates = sfmlInput.GetPressed();
			for (const auto& key : keyStates) {
				GameInput input = gamepad.ToGameInput(key);
				if (input != GameInput::Invalid) {
					auto duplicate = GameUtils::VectorSearch(currentSnapshot, input);
					if (duplicate == currentSnapshot.end()) currentSnapshot.push_back(input);
				}
			}
		}
	}

	void InputHandler::FireCallbacks(const std::vector<GameInput>& inputs, OnKey type) {
		InputVecMap& consumers = consumingObservers[type];
		for (const auto& input : inputs) {
			// If any consuming callbacks exist, general ones will not be fired
			bool fireGeneral = true;
			auto vecIter = consumers.find(input);
			if (vecIter != consumers.end()) {
				auto& vector = vecIter->second;
				GameUtils::CleanVector<OnInput>(vector, [](OnInput& Callback) { return !Callback.IsAlive(); });
				// Consuming callbacks exist; fire last/latest one
				if (!vector.empty()) {
					fireGeneral = false;
					auto iter = vector.rbegin();
					if (iter != vector.rend()) {
						(*iter)();
					}
				}
			}
			// No consumers exist. Fire all general callbacks
			if (fireGeneral) {
				InputMap& general = generalObservers[type];
				auto iter = general.find(input);
				if (iter != general.end()) {
					iter->second();
				}
			}
		}
	}

	void InputHandler::FireInput() {
		std::vector<GameInput> onPressed, onHeld, onReleased;
		
		// Build OnPressed and OnHeld vectors
		for (auto input : currentSnapshot) {
			auto search = GameUtils::VectorSearch(previousSnapshot, input);
			if (search != previousSnapshot.end()) {
				onHeld.push_back(input);
			}
			else {
				onPressed.push_back(input);
			}
		}
		
		// Build OnReleased => all GameInputs in previousSnapshot that aren't in onHeld
		onReleased = previousSnapshot;
		onReleased.erase(std::remove_if(onReleased.begin(), onReleased.end(),
			[&onHeld](GameInput input) -> bool {
				for (auto compare : onHeld) {
					if (compare == input) return true;
				}
				return false;
			}
		), onReleased.end());

		if (!onPressed.empty()) FireCallbacks(onPressed, OnKey::Pressed);
		if (!onHeld.empty()) FireCallbacks(onHeld, OnKey::Held);
		if (!onReleased.empty()) FireCallbacks(onReleased, OnKey::Released);
	}
}
