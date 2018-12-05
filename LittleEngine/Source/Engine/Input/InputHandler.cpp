#include "le_stdafx.h"
#include "InputHandler.h"
#include "Engine/Logger/Logger.h"
#include "SFMLInterface/Input.h"
#include "Utils.h"
#include "../Console/DebugConsole.h"

namespace LittleEngine {
	InputHandler::InputHandler() : Object("InputHandler") {
		/* Setup Input Bindings */ {
			m_gamepad.Bind(KeyCode::Up, GameInput::Up);
			m_gamepad.Bind(KeyCode::W, GameInput::Up);
			m_gamepad.Bind(KeyCode::Down, GameInput::Down);
			m_gamepad.Bind(KeyCode::S, GameInput::Down);
			m_gamepad.Bind(KeyCode::Left, GameInput::Left);
			m_gamepad.Bind(KeyCode::A, GameInput::Left);
			m_gamepad.Bind(KeyCode::Right, GameInput::Right);
			m_gamepad.Bind(KeyCode::D, GameInput::Right);
			m_gamepad.Bind(KeyCode::Enter, GameInput::Enter);
			m_gamepad.Bind(KeyCode::Escape, GameInput::Return);
			m_gamepad.Bind(KeyCode::Tab, GameInput::Select);

			m_gamepad.Bind(KeyCode::Space, GameInput::X);
			m_gamepad.Bind(KeyCode::E, GameInput::Y);
			m_gamepad.Bind(KeyCode::Control, GameInput::LB);
			m_gamepad.Bind(KeyCode::Shift, GameInput::RB);

			m_gamepad.Bind(KeyCode::F12, GameInput::Debug0);
		}

		m_generalObservers = {
			{ OnKey::Pressed, InputMap() },
			{ OnKey::Held, InputMap() },
			{ OnKey::Released, InputMap() }
		};

		m_consumingObservers = {
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
		auto iter = GameUtils::VectorSearch(m_currentSnapshot, keyCode);
		return iter != m_currentSnapshot.end();
	}

	OnInput::Token InputHandler::Register(const GameInput& input, OnInput::Callback callback, const OnKey& type, bool bConsume) {
		// Consumers get a Delegate<> each, and go into a vector in consumingObservers
		if (bConsume) {
			// Prepare new delegate and token
			OnInput newDelegate;
			OnInput::Token token = newDelegate.Register(callback);
			InputVecMap& toModify = m_consumingObservers[type];
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
			InputMap& toModify = m_generalObservers[type];
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
		m_rawTextInput = sfmlInput.GetRawSFMLInput();
#if ENABLED(DEBUG_CONSOLE)
		if (m_rawTextInput.Contains('`')) DebugConsole::Activate(!DebugConsole::IsActive());
		if (DebugConsole::IsActive()) DebugConsole::UpdateInput(m_rawTextInput);
		else
#endif
		{
			m_previousSnapshot = m_currentSnapshot;
			m_currentSnapshot.clear();
			const std::vector<KeyState> keyStates = sfmlInput.GetPressed();
			for (const auto& key : keyStates) {
				GameInput input = m_gamepad.ToGameInput(key);
				if (input != GameInput::Invalid) {
					auto duplicate = GameUtils::VectorSearch(m_currentSnapshot, input);
					if (duplicate == m_currentSnapshot.end()) m_currentSnapshot.push_back(input);
				}
			}
		}
	}

	void InputHandler::FireCallbacks(const std::vector<GameInput>& inputs, OnKey type) {
		InputVecMap& consumers = m_consumingObservers[type];
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
				InputMap& general = m_generalObservers[type];
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
		for (auto input : m_currentSnapshot) {
			auto search = GameUtils::VectorSearch(m_previousSnapshot, input);
			if (search != m_previousSnapshot.end()) {
				onHeld.push_back(input);
			}
			else {
				onPressed.push_back(input);
			}
		}
		
		// Build OnReleased => all GameInputs in previousSnapshot that aren't in onHeld
		onReleased = m_previousSnapshot;
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
