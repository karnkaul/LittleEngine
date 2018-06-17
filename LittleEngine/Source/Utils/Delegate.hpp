#pragma once
#include <vector>
#include <functional>

// Usage: Declare a Delegate using the type that will be passed in 
// as a parameter when triggering the delegate (operator()).
// Registering object to keep received Token alive to maintain registration
// Discard received Token to unregister associated callback
template<typename T>
class Delegate {
public:
	using Callback = std::function<void(T t)>;
	using Token = std::shared_ptr<Callback>;

	// Returns shared_ptr to be owned by caller
	Token Register(Callback callback) {
		Token ptr = std::make_shared<Callback>(callback);
		this->callbacks.emplace_back(ptr);
		return ptr;
	}
	
	// Execute the callback (if it is still alive)
	void operator()(T t) {
		Cleanup();
		for (const auto& c : callbacks) {
			auto call = c.lock();
			if (call != nullptr) {
				(*call)(t);
			}
		}
	}
	
	// Returns true if any previously distributed Token is still alive
	bool IsAlive() {
		Cleanup();
		return !callbacks.empty();
	}
private:
	std::vector<std::weak_ptr<Callback> > callbacks;
	// Remove expired weak_ptrs
	void Cleanup() {
		callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(),
			[](std::weak_ptr<Callback> ptr) {
				return ptr.lock() == nullptr;
			}
		), callbacks.end());
	}
};
