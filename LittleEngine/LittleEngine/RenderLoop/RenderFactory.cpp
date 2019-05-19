#include "stdafx.h"
#include "Core/Logger.h"
#include "RenderFactory.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Debug/DebugProfiler.h"
#include "LittleEngine/Debug/Console/Tweakable.h"

namespace LittleEngine
{
using Primitive = UPtr<SFPrimitive>;
namespace
{
struct Prim
{
	Primitive p;
	SFPrimitive* pP = nullptr;
};

Prim Construct(LayerID layer)
{
	Assert(layer > 0 && layer < _LAYER_COUNT, "Invalid LayerID");
	Primitive uP = MakeUnique<SFPrimitive>(layer);
	SFPrimitive* pP = uP.get();
	return {std::move(uP), pP};
}

inline bool IsDestroyed(const Primitive& p)
{
	return p->m_bDestroyed;
}

inline void SwapState(Primitive& p)
{
	p->SwapState();
}

inline size_t GetIdx(const Primitive& p)
{
	return static_cast<size_t>(p->GetLayer());
}

inline void ReconcileState(Primitive& p)
{
	p->ReconcileState();
}
} // namespace

RenderFactory::RenderFactory()
{
	{
		Lock lock(m_mutex);
		for (auto& vec : m_active)
		{
			vec.reserve(LAYER_RESERVE);
		}
	}
	Services::ProvideRenderFactory(*this);
}

RenderFactory::~RenderFactory()
{
	u32 count = 0;
	for (auto& vec : m_active)
	{
		count += vec.size();
		vec.clear();
	}
	LOG_I("[RenderFactory] Destroyed %d primitives", count);
	Services::UnprovideRenderFactory(*this);
}

SFPrimitive* RenderFactory::New(LayerID layer)
{
	Prim prim = Construct(layer);
	m_standby.emplace_back(std::move(prim.p));
	return prim.pP;
}

Time RenderFactory::GetLastSwapTime() const
{
	return m_lastSwapTime;
}

void RenderFactory::Lock_Swap()
{
	PROFILE_FRAME("Lock Frame", Colour(255, 102, 178, 255));
	Lock lock(m_mutex);
	PROFILE_STOP("Lock Frame");
	PROFILE_FRAME("Swap Frame", Colour(204, 0, 102, 255));
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
		size_t idx = GetIdx(p);
		Assert(idx < m_active.size(), "Invalid Layer on SFPrimitive!");
		auto& vec = m_active.at(idx);
		vec.emplace_back(std::move(p));
	}
	m_standby.clear();
	m_lastSwapTime = Time::Now();
	PROFILE_STOP("Swap Frame");
}

void RenderFactory::Reconcile()
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

RenderFactory::PrimMat& RenderFactory::GetActiveRenderMatrix()
{
	return m_active;
}
} // namespace LittleEngine