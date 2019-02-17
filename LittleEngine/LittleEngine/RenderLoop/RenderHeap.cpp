#include "stdafx.h"
#include "RenderHeap.h"
#include "CoreTypes.h"
#include "LittleEngine/Services/Services.h"
#include "SFMLAPI/Rendering/GFXBuffer.h"
#include "SFMLAPI/System/SFAssets.h"
#include "Logger.h"

namespace LittleEngine
{
using Lock = std::lock_guard<std::mutex>;

SFPrimitive* RenderHeap::New()
{
	UPtr<SFPrimitive> uPrimitive = MakeUnique<SFPrimitive>();
	SFPrimitive* pPrimitive = uPrimitive.get();
	m_uPrimitives.emplace_back(std::move(uPrimitive));
	return pPrimitive;
}

SFPrimitive* RenderHeap::New(const SFPrimitiveData& data)
{
	UPtr<SFPrimitive> uPrimitive = MakeUnique<SFPrimitive>(data);
	SFPrimitive* pPrimitive = uPrimitive.get();
	m_uPrimitives.emplace_back(std::move(uPrimitive));
	return pPrimitive;
}

GFXDataFrame RenderHeap::ConstructDataFrame()
{
	Vector<SFPrimitive> primitives;
	for (auto& uObj : m_uPrimitives)
	{
		primitives.emplace_back(SFPrimitive(*uObj));
	}
	return GFXDataFrame(std::move(primitives));
}

RenderHeap::RenderHeap(GFXBuffer& buffer) : m_pBuffer(&buffer)
{
	Services::ProvideRenderHeap(*this);
}

RenderHeap::~RenderHeap()
{
	Services::Unprovide(*this);
}

void RenderHeap::Reconcile()
{
	for (auto& uPrimitive : m_uPrimitives)
	{
		uPrimitive->ReconcileState();
	}
}

bool RenderHeap::Destroy(SFPrimitive* primitive)
{
	for (auto iter = m_uPrimitives.begin(); iter != m_uPrimitives.end(); ++iter)
	{
		if (iter->get() == primitive)
		{
			m_uPrimitives.erase(std::remove(m_uPrimitives.begin(), m_uPrimitives.end(), *iter),
								m_uPrimitives.end());
#if LOG_PRIMITIVE_DESTRUCTION
			LOG_D("[RenderHeap] Primitive destroyed");
#endif
			return true;
		}
	}
	LOG_W("[RenderHeap] SFPrimitive not owned by RenderHeap!");
	return false;
}

void RenderHeap::Destroy(InitList<SFPrimitive*> pPrimitives)
{
	for (auto pPrimitive : pPrimitives)
	{
		Destroy(pPrimitive);
	}
}

void RenderHeap::DestroyAll()
{
#if DEBUG_LOGGING
	u32 cleared = m_uPrimitives.size();
#endif
	m_uPrimitives.clear();
	LOG_I("[RenderHeap] Destroyed %d Primitives", cleared);
}
} // namespace LittleEngine