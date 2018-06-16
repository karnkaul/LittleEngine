#pragma once
#include <vector>
#include <unordered_map>
#include "Engine/Object.h"
#include "Utils/Delegate.hpp"

namespace Game {
	struct KeyState;
	enum class KeyCode;
	class Input;

	class InputHandler : public Object {
	public:
		InputHandler();
		~InputHandler();
		// Store token to keep callback registered; discard it to unregister
		Delegate<const KeyState&>::Token Register(Delegate<const KeyState&>::Callback callback, KeyCode keyCode, bool consume = false);
	private:
		struct Observer {
			Delegate<const KeyState&> callback;
			bool consume;

			Observer(Delegate<const KeyState&> callback, bool consume);
		};

		friend class Engine;
		std::unordered_map<KeyCode, std::vector<Observer> > observers;

		void Cleanup(std::vector<Observer>& vec);
		void FireInput(const Input& input);
	};
}
