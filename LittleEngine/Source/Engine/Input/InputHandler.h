#pragma once
#include <vector>
#include <unordered_map>
#include "Engine/Object.h"
#include "Utils/Delegate.hpp"
#include "Gamepad.h"

namespace Game {
	struct KeyState;
	enum class KeyCode;
	using OnInput = Action;

	class InputHandler : public Object {
	public:
		InputHandler();
		~InputHandler();

		// Store token to keep callback registered; discard it to unregister
		OnInput::Token Register(OnInput::Callback callback, GameInput keyCode, bool consume = false);
	private:
		InputHandler(const InputHandler&) = delete;
		InputHandler & operator=(const InputHandler&) = delete;
		// Convenience struct
		struct InputObserver {
			OnInput callback;
			bool consume;

			InputObserver(OnInput&& callback, bool consume);
			InputObserver& operator=(InputObserver&& move);
			InputObserver(InputObserver&&) = default;
			InputObserver(const InputObserver&) = default;
		};

		friend class Engine;
		std::unordered_map<GameInput, std::vector<InputObserver> > observers;
		
		Gamepad gamepad;
		
		void SetupInputBindings();
		void Cleanup(std::vector<InputObserver>& vec);
		// Engine to call 
		void FireInput(const std::vector<KeyState>& pressedKeys);
	};
}
