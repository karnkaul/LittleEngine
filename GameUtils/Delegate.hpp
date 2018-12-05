#pragma once
#include <vector>
#include <functional>

namespace GameUtils {
	// \brief Tokenised callback with a parameter
	// Usage: Declare a Delegate using the types that will be passed in 
	// as parameters (if any) when triggering the delegate (operator()).
	// Keep received Token alive to maintain registration,
	// discard Token to unregister associated callback
	template<typename... T>
	class Delegate {
	public:
		using Callback = std::function<void(T... t)>;
		using Token = std::shared_ptr<int>;

	private:
		using WToken = std::weak_ptr<int>;
		struct Wrapper {
			Callback callback;
			WToken wToken;

			Wrapper(const Callback& callback, Token token) : callback(callback), wToken(token) {
			}
		};

		std::vector<Wrapper> m_callbacks;

	public:
		// Returns shared_ptr to be owned by caller
		Token Register(Callback callback) {
			Token token = std::make_shared<int>(static_cast<int>(m_callbacks.size()));
			m_callbacks.emplace_back(callback, token);
			return token;
		}

		// Execute alive callbacks; returns live count
		int operator()(T... t) {
			Cleanup();
			int count = 0;
			for (const auto& c : m_callbacks) {
				if (!c.wToken.expired()) {
					c.callback(t...);
					++count;
				}
			}
			return count;
		}

		// Returns true if any previously distributed Token is still alive
		bool IsAlive() {
			Cleanup();
			return !m_callbacks.empty();
		}

	private:
		// Remove expired weak_ptrs
		void Cleanup() {
			m_callbacks.erase(std::remove_if(m_callbacks.begin(), m_callbacks.end(),
				[](Wrapper& wrapper) -> bool {
					return wrapper.wToken.expired();
				}
			), m_callbacks.end());
		}
	};
}
