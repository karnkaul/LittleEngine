#pragma once
#include <mutex>
#include <functional>
#include "Core/CoreTypes.h"
#include "Core/SimpleTime.h"
#include "SFLayerID.h"

namespace LittleEngine
{
struct SFVertArr
{
	sf::VertexArray va;
	sf::RenderStates rs;

	SFVertArr(sf::VertexArray arr, sf::RenderStates* pStates = nullptr) : va(std::move(arr))
	{
		if (pStates)
		{
			rs = *pStates;
		}
	}
};

class IRenderBuffer
{
public:
	using PrimVec = Vec<UPtr<class SFPrimitive>>;
	using PrimMat = Array<PrimVec, _LAYER_COUNT>;

protected:
	std::mutex m_mutex;

public:
	virtual Time GetLastSwapTime() const = 0;
	virtual void Lock_Swap() = 0;
	
private:
	virtual PrimMat& GetActiveRenderMatrix() = 0;

private:
	friend class SFRenderer;
};
} // namespace LittleEngine