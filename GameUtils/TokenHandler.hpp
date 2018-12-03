#pragma once
#include <vector>
#include "Delegate.hpp"

namespace GameUtils {
	template<typename T>
	struct TokenHandler {
		template<typename T>
		void AddToken(T token) {
			tokens.push_back(token);
		}

		void Clear() {
			tokens.clear();
		}

	private:
		std::vector<T> tokens;
	};
}
