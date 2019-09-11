#pragma once
#include "Core/SimpleTime.h"
#include "Primitives/Primitive.h"
#include "IRenderBuffer.h"

namespace LittleEngine
{
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
