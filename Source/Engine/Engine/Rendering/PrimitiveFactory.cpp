#include "Core/LECoreGame/LECoreUtils/Logger.h"
#include "PrimitiveFactory.h"
#include "Engine/GFX.h"
namespace LittleEngine
{
PrimitiveFactory::PrimitiveFactory()
{
	for (auto& vec : m_active)
	{
		vec.reserve(LAYER_RESERVE);
	}
}

PrimitiveFactory::~PrimitiveFactory()
{
	u32 count = 0;
	for (auto& vec : m_active)
	{
		count += static_cast<u32>(vec.size());
		vec.clear();
	}
	LOG_D("[PrimitiveFactory] Destroyed %d primitives", count);
}

Time PrimitiveFactory::LastSwapTime() const
{
	return m_lastSwapTime;
}

void PrimitiveFactory::Swap()
{
	for (auto& vec : m_active)
	{
		Core::RemoveIf<UPtr<APrimitive>>(vec, [](const UPtr<APrimitive>& uP) { return uP->IsDestroyed(); });
		for (auto& uP : vec)
		{
			uP->SwapState();
		}
	}
	for (auto& uP : m_standby)
	{
		uP->SwapState();
		size_t idx = ToIdx(uP->m_layer);
		Assert(idx < m_active.size(), "Invalid Layer on SFPrimitive!");
		auto& vec = m_active.at(idx);
		vec.emplace_back(std::move(uP));
	}
	m_standby.clear();
	g_pGFX->SwapState();
	m_lastSwapTime = Time::Now();
}

void PrimitiveFactory::Reconcile()
{
	for (auto& uP : m_standby)
	{
		uP->ReconcileGameState();
	}
	for (auto& vec : m_active)
	{
		for (auto& uP : vec)
		{
			uP->ReconcileGameState();
		}
	}
	g_pGFX->Reconcile();
}

PrimitiveFactory::PrimMat& PrimitiveFactory::ActiveRenderMatrix()
{
	return m_active;
}
} // namespace LittleEngine
