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
	CircularVector()
	{
		Reset();
	}

	CircularVector(Vec<T>&& rValue) : vec(std::move(rValue))
	{
		Reset();
	}

	typename Vec<T>::const_iterator GetIter() const
	{
		return iter;
	}

	bool IsEmpty() const
	{
		return vec.empty();
	}

	void EmplaceBack(T&& val)
	{
		vec.emplace_back(std::move(val));
		Reset();
	}

	void PushBack(const T& val)
	{
		vec.push_back(val);
		Reset();
	}

	void PopBack()
	{
		vec.pop_back();
		Reset();
	}

	void Clear()
	{
		vec.clear();
		Reset();
	}

	T* GetPtr()
	{
		return &(*iter);
	}

	const T* GetPtr() const
	{
		return &(*iter);
	}

	T& GetRef()
	{
		return *iter;
	}

	const T& GetRef() const
	{
		return *iter;
	}

	T GetVal() const
	{
		return *iter;
	}

	void Increment()
	{
		++iter;
		if (iter == vec.end())
		{
			iter = vec.begin();
		}
	}

	void Decrement()
	{
		if (iter == vec.begin())
		{
			iter = vec.end();
		}
		--iter;
	}

	void Reset(bool bToLast = false)
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

	size_t Size() const
	{
		return vec.size();
	}

	void ForEach(const std::function<void(T&)>& Callback)
	{
		for (auto& t : vec)
		{
			Callback(t);
		}
	}

	void ForEach(const std::function<void(const T&)>& Callback) const
	{
		for (const auto& t : vec)
		{
			Callback(t);
		}
	}
};
} // namespace Core
