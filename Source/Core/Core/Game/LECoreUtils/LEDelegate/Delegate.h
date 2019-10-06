/*
 * == LittleEngine Delegate ==
 *   Copyright 2019 Karn Kaul
 * Features:
 *   - Header-only
 *   - Variadic template class providing `std::function<void(T...)>` (any number of parameters)
 *   - Supports multiple callback registrants (thus `void` return type for each callback)
 *   - Token based, memory safe lifetime
 * Usage:
 *   - Create a `Delegate<>` for a simple `void()` callback, or `Delegate<T...>` for passing arguments
 *   - Call `Register()` on the object and store the received `Token` to receive the callback
 *   - Invoke the object (`foo()`) to fire all callbacks; returns number of active registrants
 *   - Discard the `Token` object to unregister a callback (recommend storing as a member variable for transient lifetime objects)
 */

#pragma once
#include <algorithm>
#include <cstdint>
#include <memory>
#include <functional>

namespace LE
{
template <typename... T>
class Delegate
{
public:
	using Token = std::shared_ptr<int32_t>;
	using Callback = std::function<void(T... t)>;

private:
	using WToken = std::weak_ptr<int32_t>;

private:
	struct Wrapper
	{
		Callback callback;
		WToken wToken;
		Wrapper(Callback callback, Token token);
	};

	std::vector<Wrapper> m_callbacks;

public:
	// Returns shared_ptr to be owned by caller
	[[nodiscard]] Token Register(Callback callback);
	// Execute alive callbacks; returns live count
	uint32_t operator()(T... t);
	// Returns true if any previously distributed Token is still alive
	bool IsAlive();
	void Clear();

private:
	// Remove expired weak_ptrs
	void Cleanup();
};

template <typename... T>
Delegate<T...>::Wrapper::Wrapper(Callback callback, Token token) : callback(std::move(callback)), wToken(token)
{
}

template <typename... T>
typename Delegate<T...>::Token Delegate<T...>::Register(Callback callback)
{
	Token token = std::make_shared<int32_t>(static_cast<int32_t>(m_callbacks.size()));
	m_callbacks.emplace_back(std::move(callback), token);
	return token;
}

template <typename... T>
uint32_t Delegate<T...>::operator()(T... t)
{
	Cleanup();
	for (const auto& c : m_callbacks)
	{
		c.callback(t...);
	}
	return static_cast<uint32_t>(m_callbacks.size());
}

template <typename... T>
bool Delegate<T...>::IsAlive()
{
	Cleanup();
	return !m_callbacks.empty();
}

template <typename... T>
void Delegate<T...>::Clear()
{
	m_callbacks.clear();
}

template <typename... T>
void Delegate<T...>::Cleanup()
{
	m_callbacks.erase(
		std::remove_if(m_callbacks.begin(), m_callbacks.end(), [](Wrapper& wrapper) -> bool { return wrapper.wToken.expired(); }),
		m_callbacks.end());
}
} // namespace LE
