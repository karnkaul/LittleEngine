#pragma once
#include <algorithm>
#include <functional>
#include <tuple>
#include "Fixed.h"
#include "StdTypes.h"
#include "Maths.h"

namespace Core
{
// Erase all elements of a vector that qualify provided Predicate
// Vector and T are passed by reference
template <typename T>
void CleanVector(Vec<T>& vec, const std::function<bool(T& t)>& Predicate);

template <typename K, typename V>
void CleanMap(UMap<K, V>& map, const std::function<bool(V& v)>& Predicate);

template <typename K, typename V>
void CleanMap(Map<K, V>& map, const std::function<bool(V& v)>& Predicate);

// Given a Vec<weak_ptr<T>>, erase all elements where t.lock() == nullptr
template <typename T>
void EraseNullWeakPtrs(Vec<WPtr<T>>& vec);

// Given a Vec<T> and T& value, returns an iterator using std::find
template <typename T>
typename Vec<T>::const_iterator VectorSearch(const Vec<T>& vec, const T& value);

// Returns true if val is found, and erases it from vec
template <typename T>
bool VectorErase(Vec<T>& vec, const T& val);
} // namespace Core

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
String ToString(T input);
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
Vec<String> Tokenise(const String& input, const char delimiter, InitList<Pair<char>> escape);
// Substitutes an input set of chars with a given replacement
void SubstituteChars(String& outInput, InitList<Pair<char>> replacements);
// Returns true if str[idx - 1] = wrapper.first && str[idx + 1] == wrapper.second
bool IsCharEnclosedIn(const String& str, size_t idx, Strings::Pair<char> wrapper);
} // namespace Strings

namespace Core
{
template <typename T>
void CleanVector(Vec<T>& vec, const std::function<bool(T& t)>& Predicate)
{
	auto iter = std::remove_if(vec.begin(), vec.end(), Predicate);
	vec.erase(iter, vec.end());
}

template <typename K, typename V>
void CleanMap(UMap<K, V>& map, const std::function<bool(V& v)>& Predicate)
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
void CleanMap(Map<K, V>& map, const std::function<bool(V& v)>& Predicate)
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

template <typename T>
void EraseNullWeakPtrs(Vec<WPtr<T>>& vec)
{
	CleanVector<WPtr<T>>(vec, [](WPtr<T>& ptr) { return ptr.expired(); });
}

template <typename T>
typename Vec<T>::const_iterator VectorSearch(const Vec<T>& vec, const T& value)
{
	typename Vec<T>::const_iterator iter = std::find(vec.begin(), vec.end(), value);
	return iter;
}

template <typename T>
bool VectorErase(Vec<T>& vec, const T& val)
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

namespace Strings
{
template <typename T>
String ToString(T input)
{
	return std::to_string(input);
}
} // namespace Strings