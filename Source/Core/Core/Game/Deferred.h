#pragma once
#include <future>

namespace Core
{
template <typename T>
class Deferred final
{
private:
	std::future<T> m_future;

public:
	Deferred();
	Deferred(std::future<T>&& future);

	T Get();
	void SafeWait();

	bool IsValid() const;
	bool IsReady() const;
};

template <typename T>
Deferred<T>::Deferred() = default;

template <typename T>
Deferred<T>::Deferred(std::future<T>&& future) : m_future(std::move(future))
{
}

template <typename T>
T Deferred<T>::Get()
{
	return m_future.get();
}

template <typename T>
void Deferred<T>::SafeWait()
{
	if (IsValid())
	{
		Get();
	}
}

template <typename T>
bool Deferred<T>::IsValid() const
{
	return m_future.valid();
}

template <typename T>
bool Deferred<T>::IsReady() const
{
	return m_future.valid() && m_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
}
} // namespace Core