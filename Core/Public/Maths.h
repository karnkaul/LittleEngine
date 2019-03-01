#pragma once
#include <random>
#include "CoreTypes.h"

namespace Maths
{
using Fixed = Core::Fixed;

const Fixed PI = Fixed(3.14159265359);
const Fixed DEG_TO_RAD = Fixed(PI / 180);

// Returns val E [min, max]
template <typename T>
T Clamp(T val, T min, T max)
{
	if (val < min)
	{
		val = min;
	}
	if (val > max)
	{
		val = max;
	}
	return val;
}

// Returns val E [0, 1]
template <typename T>
T Clamp01(T val)
{
	if (val < 0)
	{
		val = 0;
	}
	if (val > 1)
	{
		val = 1;
	}
	return val;
}

// Returns val E [-1, 1]
template <typename T>
T Clamp_11(T val)
{
	if (val < -1)
	{
		val = -1;
	}
	if (val > 1)
	{
		val = 1;
	}
	return val;
}

template <typename T>
T Abs(T val)
{
	if (val < 0)
		return -val;
	return val;
}

template <typename T>
T TransformRange(T value, T oldMin, T oldMax, T newMin, T newMax)
{
	T oldRange = oldMax - oldMin;
	T newRange = newMax - newMin;
	return oldRange == 0 ? newMin : (((value - oldMin) * newRange) / oldRange) + newMin;
}

bool IsNearlyEqual(f32 lhs, f32 rhs, f32 epsilon = 0.01f);

template <typename T>
T Lerp(const T& min, const T& max, const Fixed& alpha)
{
	return min + alpha * (max - min);
}

class Random
{
private:
	std::mt19937 m_detMt;
	std::mt19937 m_NDetMt;
	std::random_device m_randomDevice;
	std::uniform_int_distribution<s32> m_distribution;

public:
	static s32 Range(s32 min, s32 max);
	static size_t Range(size_t min, size_t max);
	static Fixed Range(const Fixed& min, const Fixed& max, u32 precision = 1000);

public:
	Random(s32 min, s32 max);

	void Seed(s32 seed);
	s32 NextDeterministic();
	s32 NextNonDeterministic();
};
}