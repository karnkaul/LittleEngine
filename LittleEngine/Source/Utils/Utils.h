#pragma once
#include "Fixed.h"
#include <vector>
#include <functional>

namespace Utils {
	// Erase all elements of a vector that qualify provided Predicate
	// Vector and T are passed by reference
	template<typename T>
	void CleanVector(std::vector<T>& vec, std::function<bool(T& t)> Predicate) {
		auto iter = std::remove_if(vec.begin(), vec.end(), Predicate);
		vec.erase(iter, vec.end());
	}

	// Given a vector<weak_ptr<T>>, erase all elements where t.lock() == nullptr
	template<typename T>
	void EraseNullWeakPtrs(std::vector<std::weak_ptr<T>>& vec) {
		CleanVector<std::weak_ptr<T>>(vec, [](std::weak_ptr<T>& ptr) { return ptr.lock() == nullptr; });
	}
}

namespace Maths {
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

	namespace Random {
		// Returns [min, max]
		Fixed Range(Fixed min = 0, Fixed max = 1);
		// Returns [min, max)
		int Range(int min, int max);
	};
}
