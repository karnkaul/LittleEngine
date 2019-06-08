#pragma once
#include <random>
#include "CoreTypes.h"
#include "SimpleTime.h"

namespace Maths
{
using Fixed = Core::Fixed;
using Time = Core::Time;

const Fixed PI = Fixed(3.14159265359);
const Fixed DEG_TO_RAD = Fixed(PI / 180);
const Fixed RAD_TO_DEG = Fixed(180 / PI);

// Returns val E [min, max]
template <typename T>
T Clamp(T val, T min, T max);

// Returns val E [0, 1]
template <typename T>
T Clamp01(T val);

// Returns val E [-1, 1]
template <typename T>
T Clamp_11(T val);

template <typename T>
T Min(T lhs, T rhs);

template <typename T>
T Max(T lhs, T rhs);

template <typename T>
T Abs(T val);

template <typename T>
T TransformRange(T value, T oldMin, T oldMax, T newMin, T newMax);

template <typename T>
T Lerp(T min, T max, Fixed alpha);

bool IsNearlyEqual(f32 lhs, f32 rhs, f32 epsilon = 0.01f);

Fixed ComputeAlpha(Time dt, Time totalTime);

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
	static Fixed Range(Fixed min, Fixed max, u32 precision = 1000);

public:
	Random(s32 min, s32 max);

	void Seed(s32 seed);
	s32 NextDeterministic();
	s32 NextNonDeterministic();
};

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
T Min(T lhs, T rhs)
{
	return lhs < rhs ? lhs : rhs;
}

template <typename T>
T Max(T lhs, T rhs)
{
	return lhs > rhs ? lhs : rhs;
}

template <typename T>
T Abs(T val)
{
	return val < 0 ? -val : val;
}

template <typename T>
T TransformRange(T value, T oldMin, T oldMax, T newMin, T newMax)
{
	T oldRange = oldMax - oldMin;
	T newRange = newMax - newMin;
	return oldRange == 0 ? newMin : (((value - oldMin) * newRange) / oldRange) + newMin;
}

template <typename T>
T Lerp(T min, T max, Fixed alpha)
{
	return min + alpha * (max - min);
}
} // namespace Maths