#pragma once
#include "Core/StdTypes.h"

namespace LittleEngine
{
template <typename T>
struct InputTokenHandler
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
} // namespace LittleEngine
