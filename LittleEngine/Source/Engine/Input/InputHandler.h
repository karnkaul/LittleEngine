#pragma once
#include <vector>
#include <unordered_map>
#include "Engine/Object.h"
#include "Utils/Delegate.hpp"

namespace Game {
	struct KeyState;
	enum class KeyCode;

	class InputHandler : public Object {
	public:
		InputHandler();
		~InputHandler();

		// Store token to keep callback registered; discard it to unregister
		Delegate<const KeyState&>::Token Register(Delegate<const KeyState&>::Callback callback, KeyCode keyCode, bool consume = false);
	private:
		InputHandler(const InputHandler&) = delete;
		InputHandler & operator=(const InputHandler&) = delete;
		// Convenience struct
		struct InputObserver {
			Delegate<const KeyState&> callback;
			bool consume;

			InputObserver(Delegate<const KeyState&>&& callback, bool consume);
			InputObserver& operator=(InputObserver&& move);
			InputObserver(InputObserver&&) = default;
			InputObserver(const InputObserver&) = default;
		};

		friend class Engine;
		std::unordered_map<KeyCode, std::vector<InputObserver> > observers;

		void Cleanup(std::vector<InputObserver>& vec);
		// Engine to call 
		void FireInput(const std::vector<KeyState>& pressedKeys);
	};
}
