#pragma once
#include "Core/SimpleTime.h"
#include "SFMLAPI/Rendering/Primitives/Primitive.h"
#include "SFMLAPI/Rendering/ISFRenderBuffer.h"

namespace LittleEngine
{
class RenderFactory final : public IRenderBuffer
{
private:
	static const u32 LAYER_RESERVE = 128;
	static const u32 LOCK_SLEEP_MS = 5;
	PrimMat m_active;
	PrimVec m_standby;
	Time m_lastSwapTime;

public:
	RenderFactory();
	~RenderFactory() override;
	
	template <typename T>
	T* New(LayerID layer);
	
	Time GetLastSwapTime() const override;

	void Reconcile();
	void Swap() override;

	PrimMat& GetActiveRenderMatrix() override;
};

template <typename T>
T* RenderFactory::New(LayerID layer)
{
	static_assert(IsDerived<APrimitive, T>, "T must derive from APrimitive");
	UPtr<T> uT = MakeUnique<T>(layer);
	T* pT = uT.get();
	m_standby.emplace_back(std::move(uT));
	return pT;
}
} // namespace LittleEngine