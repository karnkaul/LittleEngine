#pragma once
#include <array>
#include <vector>
#include "Core/SimpleTime.h"
#include "SFMLAPI/Rendering/Primitives/Primitive.h"

namespace LittleEngine
{
class PrimitiveFactory : private NoCopy
{
public:
	using PrimVec = std::vector<UPtr<APrimitive>>;
	using PrimMat = std::array<PrimVec, ToIdx(LayerID::_COUNT)>;

private:
	static const u32 LAYER_RESERVE = 128;
	static const u32 LOCK_SLEEP_MS = 5;
	PrimMat m_active;
	PrimVec m_standby;
	Time m_lastSwapTime;

public:
	PrimitiveFactory();
	virtual ~PrimitiveFactory();

	template <typename T>
	T* New(LayerID layer);

	Time LastSwapTime() const;

	void Reconcile();
	void Swap();

	PrimMat& ActiveRenderMatrix();
};

template <typename T>
T* PrimitiveFactory::New(LayerID layer)
{
	static_assert(IsDerived<APrimitive, T>(), "T must derive from APrimitive");
	UPtr<T> uT = std::make_unique<T>(layer);
	T* pT = uT.get();
	m_standby.emplace_back(std::move(uT));
	return pT;
}
} // namespace LittleEngine
