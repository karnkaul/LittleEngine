#pragma once
#include <functional>
#include "CoreTypes.h"
#include "SimpleTime.h"

namespace LittleEngine
{
class IRenderBuffer
{
public:
	virtual Time GetLastSwapTime() const = 0;
	virtual void Lock_Swap() = 0;
	virtual void Lock_Traverse(std::function<void(Vec<class SFPrimitive*> vec)> procedure) = 0;
};
} // namespace LittleEngine