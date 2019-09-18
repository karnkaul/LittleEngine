#pragma once
#include <functional>
#include "StdTypes.h"

namespace Core
{
// \brief Tokenised callback with a parameter
// Usage: Declare a Delegate using the types that will be passed in
// as parameters (if any) when triggering the delegate (operator()).
// Keep received Token alive to maintain registration,
// discard Token to unregister associated callback
template <typename... T>
class Delegate
{
public:
	using Callback = std::function<void(T... t)>;
	using Token = SPtr<s32>;

private:
	using WToken = std::weak_ptr<s32>;
	struct Wrapper
	{
		Callback callback;
		WToken wToken;
		Wrapper(Callback callback, Token token);
	};

	Vec<Wrapper> m_callbacks;

public:
	// Returns shared_ptr to be owned by caller
	Token Register(Callback callback);
	// Execute alive callbacks; returns live count
	u32 operator()(T... t);
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
	Token token = MakeShared<s32>(static_cast<s32>(m_callbacks.size()));
	m_callbacks.emplace_back(std::move(callback), token);
	return token;
}

template <typename... T>
u32 Delegate<T...>::operator()(T... t)
{
	Cleanup();
	for (const auto& c : m_callbacks)
	{
		c.callback(t...);
	}
	return static_cast<u32>(m_callbacks.size());
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
} // namespace Core
