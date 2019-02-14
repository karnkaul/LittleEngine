#pragma once
#include <mutex>
#include "CoreTypes.h"
#include "SFPrimitive.h"
#include "SFMLAPI/System/SFTime.h"

namespace LittleEngine
{
struct GFXDataFrame
{
private:
	using RenderLayer = Vector<SFPrimitive>;
	Array<RenderLayer, _LAYER_COUNT> layerArray;

public:
	static bool IsOutOfBounds(const Vector2& position, const Rect2& objectBounds, const Vector2& worldBounds);

	GFXDataFrame(Vector<SFPrimitive>&& primitives);

private:
	void Cull(const Vector2& cullBounds);
	Vector<SFPrimitive> CollapseAndMove();

	friend class GFXBuffer;
	friend class EngineLoop;
};

class GFXBuffer final
{
private:
	using Lock = std::lock_guard<std::mutex>;

	std::mutex m_bufferMutex;
	Vector<SFPrimitive> m_buffer0;
	Vector<SFPrimitive> m_buffer1;
	size_t m_bufferIdx;
	Time m_lastSwapTime;

public:
	GFXBuffer();

	Time GetLastSwapTime() const;
	void Lock_Swap(GFXDataFrame&& newFrame, const Vector2& cullBounds);
	void Lock_Traverse(Function(void(Vector<SFPrimitive>& vec)) Procedure);

private:
	Vector<SFPrimitive>* GetInactiveBuffer();
	Vector<SFPrimitive>& ReferenceActiveBuffer();
};
} // namespace LittleEngine
