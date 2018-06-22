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

		bool IsKeyPressed(GameInput keyCode) const;
		// Store token to keep callback registered; discard it to unregister
		OnInput::Token OnPressed(GameInput input, OnInput::Callback callback, bool consume = false);
		OnInput::Token OnHeld(GameInput input, OnInput::Callback callback, bool consume = false);
		OnInput::Token OnReleased(GameInput input, OnInput::Callback callback, bool consume = false);
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
		std::unordered_map<GameInput, std::vector<InputObserver> > onHeldObservers;
		std::unordered_map<GameInput, std::vector<InputObserver> > onPressedObservers;
		std::unordered_map<GameInput, std::vector<InputObserver> > onReleasedObservers;
		std::vector<GameInput> currentSnapshot;
		std::vector<GameInput> previousSnapshot;
		Gamepad gamepad;
		
		void SetupInputBindings();
		void Cleanup(std::vector<InputObserver>& vec);
		OnInput::Token Register(std::unordered_map<GameInput, std::vector<InputObserver> >& map, OnInput::Callback callback, GameInput keyCode, bool consume);
		void FireCallbacks(const std::vector<GameInput>& inputs, std::unordered_map<GameInput, std::vector<InputObserver> >& map);
		// Engine to call 
		void CaptureState(const std::vector<KeyState>& pressedKeys);
		void FireInput();
	};
}
