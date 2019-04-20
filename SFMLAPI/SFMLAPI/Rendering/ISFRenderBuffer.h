#pragma once
#include <functional>
#include "CoreTypes.h"
#include "SimpleTime.h"

// TODO: Remove
#include "SFMLAPI/Rendering/SFPrimitive.h"

namespace LittleEngine
{
class IRenderBuffer
{
public:
	virtual Time GetLastSwapTime() const = 0;
	// TODO: Make pure
	virtual void Lock_Swap()
	{
	}
	// TODO: Remove
	virtual void Lock_Swap(Vec<SFPrimitive>&)
	{
	}

	virtual void Lock_Traverse(std::function<void(Vec<SFPrimitive*> vec)> procedure) = 0;
};
} // namespace LittleEngine