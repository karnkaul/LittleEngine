#include "stdafx.h"
#include "Core/Logger.h"
#include "PrimitiveFactory.h"

namespace LittleEngine
{
using Primitive = UPtr<APrimitive>;
namespace
{
struct Prim
{
	Primitive p;
	APrimitive* pP = nullptr;
};

inline bool IsDestroyed(const Primitive& p)
{
	return p->IsDestroyed();
}

inline void SwapState(Primitive& p)
{
	p->SwapState();
}

inline size_t GetIdx(const Primitive& p)
{
	return ToIdx(p->m_layer);
}

inline void ReconcileState(Primitive& p)
{
	p->ReconcileGameState();
}
} // namespace

PrimitiveFactory::PrimitiveFactory()
{
	{
		Lock lock(m_mutex);
		for (auto& vec : m_active)
		{
			vec.reserve(LAYER_RESERVE);
		}
	}
}

PrimitiveFactory::~PrimitiveFactory()
{
	u32 count = 0;
	for (auto& vec : m_active)
	{
		count += vec.size();
		vec.clear();
	}
	LOG_D("[PrimitiveFactory] Destroyed %d primitives", count);
}

Time PrimitiveFactory::GetLastSwapTime() const
{
	return m_lastSwapTime;
}

void PrimitiveFactory::Swap()
{
	for (auto& vec : m_active)
	{
		Core::RemoveIf<Primitive>(vec, [](const Primitive& p) { return IsDestroyed(p); });
		for (auto& p : vec)
		{
			SwapState(p);
		}
	}
	for (auto& p : m_standby)
	{
		SwapState(p);
		size_t idx = GetIdx(p);
		Assert(idx < m_active.size(), "Invalid Layer on SFPrimitive!");
		auto& vec = m_active.at(idx);
		vec.emplace_back(std::move(p));
	}
	m_standby.clear();
	m_lastSwapTime = Time::Now();
}

void PrimitiveFactory::Reconcile()
{
	for (auto& p : m_standby)
	{
		ReconcileState(p);
	}
	for (auto& vec : m_active)
	{
		for (auto& p : vec)
		{
			ReconcileState(p);
		}
	}
}

PrimitiveFactory::PrimMat& PrimitiveFactory::GetActiveRenderMatrix()
{
	return m_active;
}
} // namespace LittleEngine