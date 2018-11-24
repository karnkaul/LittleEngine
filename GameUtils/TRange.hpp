#pragma once

namespace GameUtils {
	// \brief Struct to store two values as a range
	template<typename T>
	struct TRange {
		T min;
		T max;

		TRange(const T& val) : min(val), max(val) {}
		TRange(const T& min, const T& max) : min(min), max(max) {}

		bool IsFuzzy() const {
			return min != max;
		}
	};
}
