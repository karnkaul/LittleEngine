#pragma once
#include <random>
#include "Fixed.h"
#include "SimpleTime.h"

namespace Maths
{
using Fixed = Core::Fixed;
using Time = Core::Time;

constexpr f32 EPSILON = 0.001f;
const Fixed PI(3.14159265359);
const Fixed DEG_TO_RAD(PI / 180);
const Fixed RAD_TO_DEG(180 / PI);

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

template <typename T>
T Scale(T val, f32 coeff);

bool IsNearlyEqual(f32 lhs, f32 rhs, f32 epsilon = EPSILON);

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
	s32 Next(bool bDeterministic);
};

template <typename T>
inline T Clamp(T val, T min, T max)
{
	return (val < min) ? min : (val > max) ? max : val;
}

template <typename T>
inline T Clamp01(T val)
{
	return (val < T(0)) ? T(0) : (val > T(1)) ? T(1) : val;
}

template <typename T>
inline T Clamp_11(T val)
{
	return (val < T(-1)) ? T(-1) : (val > T(1)) ? T(1) : val;
}

template <typename T>
inline T Min(T lhs, T rhs)
{
	return lhs < rhs ? lhs : rhs;
}

template <typename T>
T Max(T lhs, T rhs)
{
	return lhs > rhs ? lhs : rhs;
}

template <typename T>
inline T Abs(T val)
{
	return val < T(0) ? -val : val;
}

template <typename T>
T TransformRange(T value, T oldMin, T oldMax, T newMin, T newMax)
{
	T oldRange = oldMax - oldMin;
	T newRange = newMax - newMin;
	return oldRange == T(0) ? newMin : (((value - oldMin) * newRange) / oldRange) + newMin;
}

template <typename T>
inline T Lerp(T min, T max, Fixed alpha)
{
	return min + alpha * (max - min);
}

template <typename T>
inline T Scale(T val, f32 coeff)
{
	return static_cast<T>(static_cast<f32>(val) * coeff);
}
} // namespace Maths
