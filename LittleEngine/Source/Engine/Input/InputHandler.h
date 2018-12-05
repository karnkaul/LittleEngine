#pragma once
#include <vector>
#include <array>
#include <unordered_map>
#include "Engine/Object.h"
#include "Delegate.hpp"
#include "Gamepad.h"
#include "SFMLInterface/Input.h"

namespace LittleEngine {
	struct KeyState;
	enum class KeyCode;
	using OnInput = GameUtils::Delegate<>;

	enum class OnKey { Pressed, Held, Released };

	// \brief Handles Tokenised callback subscription towards Gamepad Input
	class InputHandler final : public Object {
	private:
		using InputVecMap = std::unordered_map<GameInput, std::vector<OnInput>>;
		using InputMap = std::unordered_map<GameInput, OnInput>;

		RawTextInput m_rawTextInput;
		Gamepad m_gamepad;
		std::unordered_map<OnKey, InputMap> m_generalObservers;		// All general observers receive callbacks
		std::unordered_map<OnKey, InputVecMap> m_consumingObservers;	// Only latest consuming observer receives callback
		std::vector<GameInput> m_currentSnapshot;
		std::vector<GameInput> m_previousSnapshot;

	public:
		InputHandler();
		~InputHandler();

		bool IsKeyPressed(const GameInput& keyCode) const;
		// \brief Register a callback for an input here
		// @returns Token: Store to keep callback registered; discard it to unregister
		OnInput::Token Register(const GameInput& input, OnInput::Callback callback, const OnKey& type, bool consume = false);
	
	private:
		InputHandler(const InputHandler&) = delete;
		InputHandler & operator=(const InputHandler&) = delete;
		
		void ProcessInput(const Input& sfmlInput);
		void FireCallbacks(const std::vector<GameInput>& inputs, OnKey type);
		// Engine to call
		void FireInput();

		friend class Engine;
	};
}
