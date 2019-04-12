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

	TRange(T val);
	TRange(T min, T max);

	bool IsFuzzy() const;
	T Lerp(Fixed alpha) const;
	void Reset(T current);
	void Update(T current);
};

template <typename T>
TRange<T>::TRange(T val) : min(val), max(val)
{
}

template <typename T>
TRange<T>::TRange(T min, T max) : min(std::move(min)), max(std::move(max))
{
}

template <typename T>
bool TRange<T>::IsFuzzy() const
{
	return min != max;
}

template <typename T>
T TRange<T>::Lerp(Fixed alpha) const
{
	return Maths::template Lerp<T>(min, max, alpha);
}

template <typename T>
void TRange<T>::Reset(T current)
{
	min = current;
	max = current;
}

template <typename T>
void TRange<T>::Update(T current)
{
	min = max;
	max = std::move(current);
}
} // namespace Core
