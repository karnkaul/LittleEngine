#pragma once
#include "Core/Game/LECoreUtils/StdTypes.h"

namespace LittleEngine
{
template <typename T>
struct TokenHandler
{
private:
	std::vector<T> tokens;

public:
	void AddToken(T token)
	{
		tokens.emplace_back(std::move(token));
	}

	void Clear()
	{
		tokens.clear();
	}
};
} // namespace LittleEngine
