#pragma once
#include <mutex>
#include <functional>
#include "CoreTypes.h"
#include "SimpleTime.h"
#include "SFLayerID.h"

namespace LittleEngine
{
struct SFVertArr
{
	sf::VertexArray va;
	sf::RenderStates rs;

	SFVertArr(const sf::VertexArray& arr, sf::RenderStates* pStates = nullptr) : va(arr)
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

// TODO: Make proper API
public:
	Vec<SFVertArr> m_vertArrs;

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