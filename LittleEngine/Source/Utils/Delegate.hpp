#pragma once
#include <functional>

template<typename T>
class Delegate {
public:
	using Callback = std::function<void(T t)>;
	using Token = std::shared_ptr<Callback>;
	Token Register(Callback callback) {
		Token ptr = std::make_shared<Callback>(callback);
		this->callback = ptr;
		return ptr;
	}
	void operator()(T t) {
		Token ptr;
		if ((ptr = callback.lock()) != nullptr) {
			(*ptr)(t);
		}
	}
	bool IsAlive() const {
		return callback.lock() != nullptr;
	}
private:
	std::weak_ptr<Callback> callback;
};
