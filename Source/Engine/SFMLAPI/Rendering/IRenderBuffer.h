#pragma once
#include "Core/CoreTypes.h"
#include "Core/SimpleTime.h"
#include "LayerID.h"

namespace LittleEngine
{
class IRenderBuffer
{
public:
	using PrimVec = Vec<UPtr<class APrimitive>>;
	using PrimMat = Array<PrimVec, ToIdx(LayerID::_COUNT)>;

public:
	virtual ~IRenderBuffer() = default;

	virtual Time LastSwapTime() const = 0;
	virtual void Swap() = 0;

	virtual PrimMat& ActiveRenderMatrix() = 0;
};
} // namespace LittleEngine
