#pragma once
#include "Fixed.h"
#include "StdTypes.h"
#include <algorithm>
#include <tuple>

namespace Core
{
// Erase all elements of a vector that qualify provided Predicate
// Vector and T are passed by reference
template <typename T>
void CleanVector(Vector<T>& vec, Function(bool(T& t)) Predicate)
{
	auto iter = std::remove_if(vec.begin(), vec.end(), Predicate);
	vec.erase(iter, vec.end());
}

template <typename K, typename V>
void CleanMap(UMap<K, V>& map, Function(bool(V& v)) Predicate)
{
	for (auto iter = map.begin(); iter != map.end();)
	{
		if (Predicate(iter->second))
		{
			iter = map.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

template <typename K, typename V>
void CleanMap(Map<K, V>& map, Function(bool(V& v)) Predicate)
{
	for (auto iter = map.begin(); iter != map.end();)
	{
		if (Predicate(iter->second))
		{
			iter = map.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

// Given a vector<weak_ptr<T>>, erase all elements where t.lock() == nullptr
template <typename T>
void EraseNullWeakPtrs(Vector<WPtr<T>>& vec)
{
	CleanVector<WPtr<T>>(vec, [](WPtr<T>& ptr) { return ptr.expired(); });
}

// Given a vector<T> and T& value, returns an iterator using std::find
template <typename T>
typename Vector<T>::const_iterator VectorSearch(const Vector<T>& vec, const T& value)
{
	typename Vector<T>::const_iterator iter = std::find(vec.begin(), vec.end(), value);
	return iter;
}

// Returns true if val is found, and erases it from vec
template <typename T>
bool VectorErase(Vector<T>& vec, const T& val)
{
	auto iter = std::find(vec.begin(), vec.end(), val);
	if (iter != vec.end())
	{
		std::swap(*iter, vec.back());
		vec.pop_back();
		return true;
	}
	return false;
}
} // namespace Core

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
T TransformRange(T val, T oldMin, T oldMax, T newMin, T newMax)
{
	T oldRange = oldMax - oldMin;
	T newRange = newMax - newMin;
	return oldRange == 0 ? newMin : (((value - oldMin) * newRange) / oldRange) + newMin;
}

namespace Random
{
// Returns [min, max]
Fixed Range(const Fixed& min = 0, const Fixed& max = 1);
// Returns [min, max)
s32 Range(s32 min, s32 max);
// Returns [min, max)
size_t Range(size_t min, size_t max);
}; // namespace Random

template <typename T>
T Lerp(const T& min, const T& max, const Fixed& alpha)
{
	return min + alpha * (max - min);
}
} // namespace Maths

namespace Strings
{
using Fixed = Core::Fixed;
// ASCII only
String ToLower(const String& lhs);

bool ToBool(const String& input, bool defaultValue = false);
s32 ToS32(const String& input, s32 defaultValue = -1);
f32 ToF32(const String& input, f32 defaultValue = -1.0f);
f64 ToF64(const String& input, f64 defaultValue = -1.0f);

template <typename T>
String ToString(T input)
{
	return std::to_string(input);
}
String ToString(bool bInput);

template <typename T>
struct Pair
{
	T first;
	T second;

	Pair(typename std::pair<T, T>&& pair) : first(pair.first), second(pair.second)
	{
	}
	Pair(T first, T second) : first(first), second(second)
	{
	}
	Pair(T value) : first(value), second(value)
	{
	}
	operator typename std::pair<T, T>()
	{
		return typename std::make_pair<T, T>(first, second);
	}
};

// Slices a string into a pair via the first occurence of a delimiter
Pair<String> Bisect(const String& input, char delimiter);
// Removes all occurrences of toRemove from outInput
void RemoveChars(String& outInput, InitList<char> toRemove);
// Removes all tabs and spaces
void RemoveWhitespace(String& outInput);
// Tokenises a string via a delimiter, skipping over any delimiters within escape characters
Vector<String> Tokenise(const String& input, const char delimiter, InitList<Pair<char>> escape);
// Substitutes an input set of chars with a given replacement
void SubstituteChars(String& outInput, InitList<Pair<char>> replacements);
// Returns true if str[idx - 1] = wrapper.first && str[idx + 1] == wrapper.second
bool IsCharEnclosedIn(const String& str, size_t idx, Strings::Pair<char> wrapper);
} // namespace Strings
