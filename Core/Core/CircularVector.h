#include "CoreTypes.h"

#pragma once
namespace Core
{
template <typename T>
struct CircularVector
{
private:
	Vec<T> vec;
	typename Vec<T>::iterator iter;

public:
	CircularVector();
	CircularVector(Vec<T> vec);

	typename Vec<T>::const_iterator GetIter() const;
	bool IsEmpty() const;
	void EmplaceBack(T&& val);
	void PushBack(const T& val);
	void PopBack();
	void Clear();
	T* GetPtr();
	const T* GetPtr() const;
	T& GetRef();
	const T& GetRef() const;
	T GetVal() const;
	void Increment();
	void Decrement();
	void Reset(bool bToLast = false);
	size_t Size() const;
	void ForEach(std::function<void(T&)> callback);
	void ForEach(std::function<void(const T&)> callback) const;
};

template<typename T>
CircularVector<T>::CircularVector()
{
	Reset();
}

template <typename T>
CircularVector<T>::CircularVector(Vec<T> vec) : vec(std::move(vec))
{
	Reset();
}

template <typename T>
typename Vec<T>::const_iterator CircularVector<T>::GetIter() const
{
	return iter;
}

template <typename T>
bool CircularVector<T>::IsEmpty() const
{
	return vec.empty();
}

template <typename T>
void CircularVector<T>::EmplaceBack(T&& val)
{
	vec.emplace_back(std::move(val));
	Reset();
}

template <typename T>
void CircularVector<T>::PushBack(const T& val)
{
	vec.push_back(val);
	Reset();
}

template <typename T>
void CircularVector<T>::PopBack()
{
	vec.pop_back();
	Reset();
}

template <typename T>
void CircularVector<T>::Clear()
{
	vec.clear();
	Reset();
}

template <typename T>
T* CircularVector<T>::GetPtr()
{
	return &(*iter);
}

template <typename T>
const T* CircularVector<T>::GetPtr() const
{
	return &(*iter);
}

template <typename T>
T& CircularVector<T>::GetRef()
{
	return *iter;
}

template <typename T>
const T& CircularVector<T>::GetRef() const
{
	return *iter;
}

template <typename T>
T CircularVector<T>::GetVal() const
{
	return *iter;
}

template <typename T>
void CircularVector<T>::Increment()
{
	++iter;
	if (iter == vec.end())
	{
		iter = vec.begin();
	}
}

template <typename T>
void CircularVector<T>::Decrement()
{
	if (iter == vec.begin())
	{
		iter = vec.end();
	}
	--iter;
}

template <typename T>
void CircularVector<T>::Reset(bool bToLast)
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
size_t CircularVector<T>::Size() const
{
	return vec.size();
}

template <typename T>
void CircularVector<T>::ForEach(std::function<void(T&)> callback)
{
	for (auto& t : vec)
	{
		callback(t);
	}
}

template <typename T>
void CircularVector<T>::ForEach(std::function<void(const T&)> callback) const
{
	for (const auto& t : vec)
	{
		callback(t);
	}
}
} // namespace Core
