#pragma once
#include "Core/SimpleTime.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "SFMLAPI/Rendering/ISFRenderBuffer.h"

namespace LittleEngine
{
class RenderFactory final : public IRenderBuffer
{
private:
	using Lock = std::lock_guard<std::mutex>;

private:
	static const u32 LAYER_RESERVE = 128;
	static const u32 LOCK_SLEEP_MS = 5;
	PrimMat m_active;
	PrimVec m_standby;
	Time m_lastSwapTime;

public:
	RenderFactory();
	~RenderFactory() override;
	
	SFPrimitive* New(LayerID layer);
	
	Time GetLastSwapTime() const override;

	void Reconcile();
	void Swap() override;

	PrimMat& GetActiveRenderMatrix() override;
};
} // namespace LittleEngine