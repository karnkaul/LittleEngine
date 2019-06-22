#pragma once
#include "Core/CoreTypes.h"
#include "Core/SimpleTime.h"
#include "SFLayerID.h"

namespace LittleEngine
{
class IRenderBuffer
{
public:
	using PrimVec = Vec<UPtr<class APrimitive>>;
	using PrimMat = Array<PrimVec, _LAYER_COUNT>;

public:
	std::mutex m_mutex;

public:
	virtual ~IRenderBuffer() = default;

	virtual Time GetLastSwapTime() const = 0;
	virtual void Swap() = 0;
	
	virtual PrimMat& GetActiveRenderMatrix() = 0;
};
} // namespace LittleEngine