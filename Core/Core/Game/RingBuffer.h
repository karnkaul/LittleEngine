#include <vector>
#include <functional>
#include "GameTypes/StdTypes.h"

#pragma once
namespace Core
{
template <typename T>
struct RingBuffer
{
private:
	std::vector<T> vec;
	typename std::vector<T>::iterator iter;

public:
	RingBuffer();
	RingBuffer(std::vector<T> vec);

	typename std::vector<T>::const_iterator Iter() const;
	bool IsEmpty() const;
	void EmplaceBack(T&& val);
	void PushBack(const T& val);
	void PopBack();
	void Clear();
	T* Ptr();
	const T* Ptr() const;
	T& Ref();
	const T& Ref() const;
	T Val() const;
	void Increment();
	void Decrement();
	void Reset(bool bToLast = false);
	size_t Size() const;
	void ForEach(std::function<void(T&)> callback);
	void ForEach(std::function<void(const T&)> callback) const;
};

template <typename T>
RingBuffer<T>::RingBuffer()
{
	Reset();
}

template <typename T>
RingBuffer<T>::RingBuffer(std::vector<T> vec) : vec(std::move(vec))
{
	Reset();
}

template <typename T>
typename std::vector<T>::const_iterator RingBuffer<T>::Iter() const
{
	return iter;
}

template <typename T>
bool RingBuffer<T>::IsEmpty() const
{
	return vec.empty();
}

template <typename T>
void RingBuffer<T>::EmplaceBack(T&& val)
{
	vec.emplace_back(std::move(val));
	Reset();
}

template <typename T>
void RingBuffer<T>::PushBack(const T& val)
{
	vec.push_back(val);
	Reset();
}

template <typename T>
void RingBuffer<T>::PopBack()
{
	vec.pop_back();
	Reset();
}

template <typename T>
void RingBuffer<T>::Clear()
{
	vec.clear();
	Reset();
}

template <typename T>
T* RingBuffer<T>::Ptr()
{
	return &(*iter);
}

template <typename T>
const T* RingBuffer<T>::Ptr() const
{
	return &(*iter);
}

template <typename T>
T& RingBuffer<T>::Ref()
{
	return *iter;
}

template <typename T>
const T& RingBuffer<T>::Ref() const
{
	return *iter;
}

template <typename T>
T RingBuffer<T>::Val() const
{
	return *iter;
}

template <typename T>
void RingBuffer<T>::Increment()
{
	++iter;
	if (iter == vec.end())
	{
		iter = vec.begin();
	}
}

template <typename T>
void RingBuffer<T>::Decrement()
{
	if (iter == vec.begin())
	{
		iter = vec.end();
	}
	--iter;
}

template <typename T>
void RingBuffer<T>::Reset(bool bToLast)
{
	if (bToLast && !vec.empty())
	{
		iter = vec.end();
		Decrement();
	}
	else
	{
		iter = vec.begin();
	}
}

template <typename T>
size_t RingBuffer<T>::Size() const
{
	return vec.size();
}

template <typename T>
void RingBuffer<T>::ForEach(std::function<void(T&)> callback)
{
	for (auto& t : vec)
	{
		callback(t);
	}
}

template <typename T>
void RingBuffer<T>::ForEach(std::function<void(const T&)> callback) const
{
	for (const auto& t : vec)
	{
		callback(t);
	}
}
} // namespace Core
