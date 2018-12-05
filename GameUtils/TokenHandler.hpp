#pragma once
#include <vector>
#include "Delegate.hpp"

namespace GameUtils {
	template<typename T>
	struct TokenHandler {
	private:
		std::vector<T> tokens;

	public:
		template<typename T>
		void AddToken(T token) {
			tokens.push_back(token);
		}

		void Clear() {
			tokens.clear();
		}
	};
}
