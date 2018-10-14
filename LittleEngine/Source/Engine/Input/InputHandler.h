#pragma once
#include <vector>
#include <unordered_map>
#include "Engine/Object.h"
#include "Utils/Delegate.hpp"
#include "Gamepad.h"

namespace LittleEngine {
	struct KeyState;
	enum class KeyCode;
	using OnInput = Utils::Delegate<>;

	enum class OnKey { Pressed, Held, Released };

	// \brief Handles Tokenised callback subscription towards Gamepad Input
	class InputHandler final : public Object {
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
		
		// Convenience struct
		struct InputObserver {
			OnInput callback;
			OnKey type;
			bool consume;

			InputObserver(OnInput&& callback, bool consume, const OnKey& type);
			InputObserver& operator=(InputObserver&& move);
			InputObserver(InputObserver&&) = default;
			InputObserver(const InputObserver&) = default;
		};

		friend class Engine;
		std::unordered_map<GameInput, std::vector<InputObserver>> inputObservers;
		std::vector<GameInput> currentSnapshot;
		std::vector<GameInput> previousSnapshot;
		std::string rawTextInput;
		
		Gamepad gamepad;
		
		void Cleanup(std::vector<InputObserver>& vec);
		OnInput::Token Register(std::unordered_map<GameInput, std::vector<InputObserver>>& map, OnInput::Callback callback, GameInput keyCode, bool consume);
		void FireCallbacks(const std::vector<GameInput>& inputs, std::unordered_map<GameInput, std::vector<InputObserver>>& map, OnKey type);
		// Engine to call 
		void CaptureState(const std::vector<KeyState>& pressedKeys);
		// Engine to call
		void CaptureRawText(const std::string& rawTextInput);
		// Engine to call
		void FireInput();
	};
}
