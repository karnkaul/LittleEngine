#pragma once
#include "StdTypes.h"

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
		tokens.emplace_back(std::move(token));
	}

	void Clear()
	{
		tokens.clear();
	}
};
} // namespace Core
