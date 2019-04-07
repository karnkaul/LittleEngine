#pragma once
#include <mutex>
#include <functional>
#include "CoreTypes.h"
#include "SimpleTime.h"
#include "SFPrimitive.h"

namespace LittleEngine
{
struct GFXDataFrame
{
private:
	using RenderLayer = Vec<SFPrimitive>;
	Array<RenderLayer, _LAYER_COUNT> layerArray;

public:
	static bool IsOutOfBounds(Vector2 position, const Rect2& objectBounds, Vector2 worldBounds);

	GFXDataFrame(Vec<SFPrimitive>&& primitives);

private:
	void Cull(Vector2 cullBounds);
	Vec<SFPrimitive> CollapseAndMove();

	friend class GFXBuffer;
	friend class EngineLoop;
};

class GFXBuffer final
{
private:
	using Lock = std::lock_guard<std::mutex>;

	std::mutex m_bufferMutex;
	Vec<SFPrimitive> m_buffer0;
	Vec<SFPrimitive> m_buffer1;
	size_t m_bufferIdx;
	Time m_lastSwapTime;

public:
	GFXBuffer();
	~GFXBuffer();

	Time GetLastSwapTime() const;
	void Lock_Swap(GFXDataFrame&& newFrame, Vector2 cullBounds);
	void Lock_Traverse(const std::function<void(Vec<SFPrimitive>& vec)>& Procedure);

private:
	Vec<SFPrimitive>* GetInactiveBuffer();
	Vec<SFPrimitive>& ReferenceActiveBuffer();
};
} // namespace LittleEngine
