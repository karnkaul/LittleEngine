#pragma once
#include "Core/SimpleTime.h"
#include "Primitives/Primitive.h"
#include "IRenderBuffer.h"

namespace LittleEngine
{
#if ENABLED(RENDER_STATS)
struct RenderData
{
	Time tickRate;
	Time lastRenderTime;
	u32 drawCallCount = 0;
	u32 quadCount = 0;
	u32 _quadCount_Internal = 0; // Unstable to render
	u32 staticCount = 0;
	u32 dynamicCount = 0;
	u32 fpsMax = 0;
	u32 framesPerSecond = 0;
	u32 renderFrame = 0;
	u32 gameFrame = 0;
};
extern RenderData g_renderData;
#endif

class PrimitiveFactory : public IRenderBuffer
{
private:
	static const u32 LAYER_RESERVE = 128;
	static const u32 LOCK_SLEEP_MS = 5;
	PrimMat m_active;
	PrimVec m_standby;
	Time m_lastSwapTime;

public:
	PrimitiveFactory();
	~PrimitiveFactory() override;

	template <typename T>
	T* New(LayerID layer);

	Time LastSwapTime() const override;

	void Reconcile();
	void Swap() override;

	PrimMat& ActiveRenderMatrix() override;
};

template <typename T>
T* PrimitiveFactory::New(LayerID layer)
{
	static_assert(IsDerived<APrimitive, T>(), "T must derive from APrimitive");
	UPtr<T> uT = MakeUnique<T>(layer);
	T* pT = uT.get();
	m_standby.emplace_back(std::move(uT));
	return pT;
}
} // namespace LittleEngine
