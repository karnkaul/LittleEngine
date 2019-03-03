#pragma once
#include "Delegate.hpp"
#include "StdTypes.h"
#include <vector>

namespace Core
{
template <typename T>
struct TokenHandler
{
private:
	Vec<T> tokens;

public:
	template <typename T>
	void AddToken(T token)
	{
		tokens.push_back(token);
	}

	void Clear()
	{
		tokens.clear();
	}
};
} // namespace Core
