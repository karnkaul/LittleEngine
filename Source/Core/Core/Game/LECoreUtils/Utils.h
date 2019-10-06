#pragma once
#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "StdTypes.h"
#include "Maths.h"

namespace Core
{
// Returns true if val is found, and erases it from vec
template <typename T>
bool Remove(std::vector<T>& vec, const T& val);

// Erase all elements of a vector that qualify provided Predicate
// Vector and T are passed by reference
template <typename T>
size_t RemoveIf(std::vector<T>& vec, std::function<bool(T& t)> predicate);

template <typename T>
size_t RemoveIf(std::list<T>& list, std::function<bool(T& t)> predicate);

template <typename K, typename V>
size_t RemoveIf(std::unordered_map<K, V>& map, std::function<bool(V& v)> predicate);

template <typename K, typename V>
size_t RemoveIf(std::map<K, V>& map, std::function<bool(V& v)> predicate);

// Given a std::vector<weak_ptr<T>>, erase all elements where t.lock() == nullptr
template <typename T>
void EraseNullWeakPtrs(std::vector<std::weak_ptr<T>>& vec);

// Given a std::vector<T> and T& value, returns an iterator using std::find
template <typename T>
typename std::vector<T>::const_iterator Search(const std::vector<T>& vec, const T& value);

// Returns size in B / KiB / MiB / GiB
std::pair<f32, std::string_view> FriendlySize(u64 byteCount);
} // namespace Core

namespace Strings
{
using Fixed = Core::Fixed;
// ASCII only
void ToLower(std::string& outString);
void ToUpper(std::string& outString);

bool ToBool(std::string input, bool defaultValue = false);
s32 ToS32(std::string input, s32 defaultValue = -1);
f32 ToF32(std::string input, f32 defaultValue = -1.0f);
f64 ToF64(std::string input, f64 defaultValue = -1.0);

template <typename T>
std::string ToString(T input);
std::string ToString(bool bInput);
template <typename T>
std::vector<std::string> ToString(const std::vector<T>& vec, std::string prefix = "", std::string suffix = "");

std::string ToText(std::vector<u8> rawBuffer);

// Slices a string into a pair via the first occurence of a delimiter
std::pair<std::string, std::string> Bisect(std::string_view input, char delimiter);
// Removes all occurrences of toRemove from outInput
void RemoveChars(std::string& outInput, std::initializer_list<char> toRemove);
// Removes leading and trailing characters
void Trim(std::string& outInput, std::initializer_list<char> toRemove);
// Removes all tabs and spaces
void RemoveWhitespace(std::string& outInput);
// Tokenises a string via a delimiter, skipping over any delimiters within escape characters
std::vector<std::string> Tokenise(std::string_view s, char delimiter, std::initializer_list<std::pair<char, char>> escape);
// Substitutes an input set of chars with a given replacement
void SubstituteChars(std::string& outInput, std::initializer_list<std::pair<char, char>> replacements);
// Returns true if str[idx - 1] = wrapper.first && str[idx + 1] == wrapper.second
bool IsCharEnclosedIn(std::string_view str, size_t idx, std::pair<char, char> wrapper);
} // namespace Strings

namespace Core
{
template <typename T>
bool Remove(std::vector<T>& vec, const T& val)
{
	auto iter = std::remove(vec.begin(), vec.end(), val);
	bool bFound = iter != vec.end();
	vec.erase(iter, vec.end());
	return bFound;
}

template <typename T>
size_t RemoveIf(std::vector<T>& vec, std::function<bool(T& t)> predicate)
{
	size_t before = vec.size();
	auto iter = std::remove_if(vec.begin(), vec.end(), predicate);
	vec.erase(iter, vec.end());
	return before - vec.size();
}

template <typename T>
size_t RemoveIf(std::list<T>& list, std::function<bool(T& t)> predicate)
{
	size_t before = list.size();
	auto iter = std::remove_if(list.begin(), list.end(), predicate);
	list.erase(iter, list.end());
	return before - list.size();
}

template <typename K, typename V>
size_t RemoveIf(std::unordered_map<K, V>& map, std::function<bool(V& v)> predicate)
{
	size_t before = map.size();
	for (auto iter = map.begin(); iter != map.end();)
	{
		iter = (predicate(iter->second)) ? map.erase(iter) : ++iter;
	}
	return before - map.size();
}

template <typename K, typename V>
size_t RemoveIf(std::map<K, V>& map, std::function<bool(V& v)> predicate)
{
	size_t before = map.size();
	for (auto iter = map.begin(); iter != map.end();)
	{
		iter = (predicate(iter->second)) ? map.erase(iter) : ++iter;
	}
	return before - map.size();
}

template <typename T>
void EraseNullWeakPtrs(std::vector<std::weak_ptr<T>>& vec)
{
	Remove<std::weak_ptr<T>>(vec, [](auto& ptr) { return ptr.expired(); });
}

template <typename T>
typename std::vector<T>::const_iterator Search(const std::vector<T>& vec, const T& value)
{
	return std::find(vec.begin(), vec.end(), value);
}
} // namespace Core

namespace Strings
{
template <typename T>
std::string ToString(T input)
{
	return std::to_string(input);
}

template <typename T>
std::vector<std::string> ToString(const std::vector<T>& vec, std::string prefix, std::string suffix)
{
	std::vector<std::string> ret;
	for (const auto& t : vec)
	{
		ret.emplace_back(prefix + ToString(t) + suffix);
	}
	return ret;
}
} // namespace Strings
