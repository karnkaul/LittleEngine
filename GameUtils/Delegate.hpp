#pragma once
#include <vector>
#include <functional>

namespace GameUtils {
	// \brief Tokenised callback with a parameter
	// Usage: Declare a Delegate using the type that will be passed in 
	// as a parameter when triggering the delegate (operator()).
	// Registering object to keep received Token alive to maintain registration
	// Discard received Token to unregister associated callback
	template<typename... T>
	class Delegate {
	public:
		using Callback = std::function<void(T... t)>;
		using Token = std::shared_ptr<int>;

		// Returns shared_ptr to be owned by caller
		Token Register(Callback callback) {
			Token token = std::make_shared<int>(static_cast<int>(callbacks.size()));
			callbacks.emplace_back(callback, token);
			return token;
		}

		// Execute alive callbacks; returns live count
		int operator()(T... t) {
			Cleanup();
			int count = 0;
			for (const auto& c : callbacks) {
				if (!c.token.expired()) {
					c.callback(t...);
					++count;
				}
			}
			return count;
		}

		// Returns true if any previously distributed Token is still alive
		bool IsAlive() {
			Cleanup();
			return !callbacks.empty();
		}
	private:
		struct Wrapper {
			Callback callback;
			std::weak_ptr<int> token;

			Wrapper(const Callback& callback, Token token) : callback(callback), token(token) {
			}
		};

		std::vector<Wrapper> callbacks;
		// Remove expired weak_ptrs
		void Cleanup() {
			callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(),
				[](Wrapper& wrapper) -> bool {
					return wrapper.token.expired();
				}
			), callbacks.end());
		}
	};
}
