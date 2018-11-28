#pragma once
#include "Fixed.h"
#include <vector>
#include <unordered_map>
#include <map>
#include <functional>
#include <algorithm>

namespace GameUtils {
	// Erase all elements of a vector that qualify provided Predicate
	// Vector and T are passed by reference
	template<typename T>
	void CleanVector(std::vector<T>& vec, std::function<bool(T& t)> Predicate) {
		auto iter = std::remove_if(vec.begin(), vec.end(), Predicate);
		vec.erase(iter, vec.end());
	}

	template<typename K, typename V>
	void CleanMap(std::unordered_map<K, V>& map, std::function<bool(V& v)> Predicate) {
		for (auto iter = map.begin(); iter != map.end();) {
			if (Predicate(iter->second)) {
				iter = map.erase(iter);
			}
			else {
				++iter;
			}
		}
	}

	template<typename K, typename V>
	void CleanMap(std::map<K, V>& map, std::function<bool(V& v)> Predicate) {
		for (auto iter = map.begin(); iter != map.end();) {
			if (Predicate(iter->second)) {
				iter = map.erase(iter);
			}
			else {
				++iter;
			}
		}
	}

	// Given a vector<weak_ptr<T>>, erase all elements where t.lock() == nullptr
	template<typename T>
	void EraseNullWeakPtrs(std::vector<std::weak_ptr<T>>& vec) {
		CleanVector<std::weak_ptr<T>>(vec, [](std::weak_ptr<T>& ptr) { return ptr.lock() == nullptr; });
	}

	// Given a vector<T> and T& value, returns an iterator using std::find
	template<typename T>
	typename std::vector<T>::const_iterator VectorSearch(const std::vector<T>& vec, const T& value) {
		typename std::vector<T>::const_iterator iter = std::find(vec.begin(), vec.end(), value);
		return iter;
	}

	// Returns true if val is found, and erases it from vec
	template<typename T>
	bool VectorErase(std::vector<T>& vec, const T& val) {
		auto iter = VectorSearch(vec, val);
		if (iter != vec.end()) {
			vec.erase(iter);
			return true;
		}
		return false;
	}
}

namespace Maths {
	using Fixed = GameUtils::Fixed;

	const Fixed PI = Fixed(3.14159265359);
	const Fixed DEG_TO_RAD = Fixed(PI / 180);

	// Returns val E [min, max]
	template<typename T>
	T Clamp(T val, T min, T max) {
		if (val < min) {
			val = min;
		}
		if (val > max) {
			val = max;
		}
		return val;
	}

	// Returns val E [0, 1]
	template<typename T>
	T Clamp01(T val) {
		if (val < 0) {
			val = 0;
		}
		if (val > 1) {
			val = 1;
		}
		return val;
	}

	// Returns val E [-1, 1]
	template<typename T>
	T Clamp_11(T val) {
		if (val < -1) {
			val = -1;
		}
		if (val > 1) {
			val = 1;
		}
		return val;
	}

	template<typename T>
	T Abs(T val) {
		if (val < 0) return -val;
		return val;
	}

	namespace Random {
		// Returns [min, max]
		Fixed Range(const Fixed& min = 0, const Fixed& max = 1);
		// Returns [min, max)
		int Range(int min, int max);
		// Returns [min, max)
		size_t Range(size_t min, size_t max);
	};

	Fixed Lerp(const Fixed& min, const Fixed& max, Fixed t);
}

namespace Strings {
	using Fixed = GameUtils::Fixed;
	// ASCII only
	std::string ToLower(const std::string& lhs);

	bool ToBool(const std::string& input, bool defaultValue = false);
	int ToInt(const std::string& input, int defaultValue = -1);
	double ToDouble(const std::string& input, double defaultValue = -1.0f);

	template<typename T>
	std::string ToString(T input) {
		return std::to_string(input);
	}
	std::string ToString(bool bInput);

	struct DiChar {
		char open;
		char close;

		DiChar(char identical) : open(identical), close(identical) {}
		DiChar(char open, char close) : open(open), close(close) {}
	};
	
	using Pair = std::pair<std::string, std::string>;
	// Slices a string into a pair via the first occurence of a delimiter
	Pair Slice(const std::string& input, char delimiter);
	// Removes all occurrences of toRemove from outInput
	void RemoveChars(std::string& outInput, std::initializer_list<char> toRemove);
	// Removes all tabs and spaces
	void RemoveWhitespace(std::string& outInput);
	// Tokenises a string via a delimiter, skipping over any delimiters within escape characters
	std::vector<std::string> Tokenise(const std::string& input, const char delimiter = ' ', std::initializer_list<char> escape = { '[', ']', '\"' });
	
	std::vector<std::string> Tokenise(const std::string& input, const char delimiter, std::initializer_list<DiChar> escape);
	// Substitutes an input set of chars with a given replacement
	void SubstituteChars(std::string & outInput, char replacement, std::initializer_list<char> toReplace);
}
