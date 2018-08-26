#pragma once
#include "Fixed.h"

namespace Maths {
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

	namespace Random {
		Fixed Range(Fixed min = 0, Fixed max = 1);
		int Range(int min, int max);
	};
}
