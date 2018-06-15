#pragma once
#include <vector>
#include <unordered_map>
#include <functional>
#include "Engine/Object.h"

namespace Game {
	struct KeyState;
	enum class KeyCode;
	class Input;

	class InputHandler : public Object {
	public:
		InputHandler();
		~InputHandler();
		using Callback = std::function<void(const KeyState&)>;
		using Token = std::shared_ptr<Callback>;
		// Store token to keep callback registered; discard it to unregister
		Token Register(Callback callback, KeyCode keyCode, bool consume = false);
	private:
		struct Observer {
			std::weak_ptr<Callback> callback;
			KeyCode keyCode;
			bool consume;

			Observer(std::weak_ptr<Callback> callback, KeyCode keyCode, bool consume);
		};

		friend class Engine;
		std::unordered_map<KeyCode, std::vector<Observer> > observers;

		void FireInput(const Input& input);
	};
}
