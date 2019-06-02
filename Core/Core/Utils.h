#pragma once
#include <algorithm>
#include <functional>
#include "Fixed.h"
#include "StdTypes.h"
#include "Maths.h"

namespace Core
{
// Returns true if val is found, and erases it from vec
template <typename T>
bool Remove(Vec<T>& vec, const T& val);

// Erase all elements of a vector that qualify provided Predicate
// Vector and T are passed by reference
template <typename T>
void RemoveIf(Vec<T>& vec, std::function<bool(T& t)> predicate);

template <typename K, typename V>
void RemoveIf(UMap<K, V>& map, std::function<bool(V& v)> predicate);

template <typename K, typename V>
void RemoveIf(Map<K, V>& map, std::function<bool(V& v)> predicate);

// Given a Vec<weak_ptr<T>>, erase all elements where t.lock() == nullptr
template <typename T>
void EraseNullWeakPtrs(Vec<WPtr<T>>& vec);

// Given a Vec<T> and T& value, returns an iterator using std::find
template <typename T>
typename Vec<T>::const_iterator Search(const Vec<T>& vec, const T& value);
} // namespace Core

namespace Strings
{
using Fixed = Core::Fixed;
// ASCII only
void ToLower(String& outString);

bool ToBool(String input, bool defaultValue = false);
s32 ToS32(String input, s32 defaultValue = -1);
f32 ToF32(String input, f32 defaultValue = -1.0f);
f64 ToF64(String input, f64 defaultValue = -1.0);

template <typename T>
String ToString(T input);
String ToString(bool bInput);
template <typename T>
Vec<String> ToString(const Vec<T>& vec, String prefix = "", String suffix = "");

template <typename T>
using Pair = std::pair<T, T>;

// Slices a string into a pair via the first occurence of a delimiter
Pair<String> Bisect(const String& input, char delimiter);
// Removes all occurrences of toRemove from outInput
void RemoveChars(String& outInput, InitList<char> toRemove);
// Removes leading and trailing characters
void Trim(String& outInput, InitList<char> toRemove);
// Removes all tabs and spaces
void RemoveWhitespace(String& outInput);
// Tokenises a string via a delimiter, skipping over any delimiters within escape characters
Vec<String> Tokenise(const String& s, char delimiter, InitList<Pair<char>> escape);
// Substitutes an input set of chars with a given replacement
void SubstituteChars(String& outInput, InitList<Pair<char>> replacements);
// Returns true if str[idx - 1] = wrapper.first && str[idx + 1] == wrapper.second
bool IsCharEnclosedIn(const String& str, size_t idx, Strings::Pair<char> wrapper);
} // namespace Strings

namespace Core
{
template <typename T>
bool Remove(Vec<T>& vec, const T& val)
{
	auto iter = std::remove(vec.begin(), vec.end(), val);
	bool bFound = iter != vec.end();
	vec.erase(iter, vec.end());
	return bFound;
}

template <typename T>
void RemoveIf(Vec<T>& vec, std::function<bool(T& t)> predicate)
{
	auto iter = std::remove_if(vec.begin(), vec.end(), predicate);
	vec.erase(iter, vec.end());
}

template <typename K, typename V>
void RemoveIf(UMap<K, V>& map, std::function<bool(V& v)> predicate)
{
	for (auto iter = map.begin(); iter != map.end();)
	{
		iter = (predicate(iter->second)) ? map.erase(iter) : ++iter;
	}
}

template <typename K, typename V>
void RemoveIf(Map<K, V>& map, std::function<bool(V& v)> predicate)
{
	for (auto iter = map.begin(); iter != map.end();)
	{
		iter = (predicate(iter->second)) ? map.erase(iter) : ++iter;
	}
}

template <typename T>
void EraseNullWeakPtrs(Vec<WPtr<T>>& vec)
{
	CleanVector<WPtr<T>>(vec, [](WPtr<T>& ptr) { return ptr.expired(); });
}

template <typename T>
typename Vec<T>::const_iterator Search(const Vec<T>& vec, const T& value)
{
	return std::find(vec.begin(), vec.end(), value);
}
} // namespace Core

namespace Strings
{
template <typename T>
String ToString(T input)
{
	return std::to_string(input);
}

template <typename T>
Vec<String> ToString(const Vec<T>& vec, String prefix, String suffix)
{
	Vec<String> ret;
	for (const auto& t : vec)
	{
		ret.emplace_back(prefix + ToString(t) + suffix);
	}
	return ret;
}
} // namespace Strings