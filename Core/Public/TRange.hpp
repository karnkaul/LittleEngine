#pragma once
#include "Utils.h"

namespace Core
{
// \brief Struct to store two values as a range
template <typename T>
struct TRange
{
	T min;
	T max;

	TRange(const T& val) : min(val), max(val)
	{
	}
	TRange(const T& min, const T& max) : min(min), max(max)
	{
	}

	bool IsFuzzy() const
	{
		return min != max;
	}

	T Lerp(const Fixed& alpha) const
	{
		return Maths::template Lerp<T>(min, max, alpha);
	}

	void Reset(const T& current)
	{
		min = current;
		max = current;
	}

	void Update(const T& current)
	{
		min = max;
		max = current;
	}
};
} // namespace Core
