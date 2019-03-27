#include "stdafx.h"
#include "Logger.h"
#include "UIContext.h"
#include "UIManager.h"
#include "LittleEngine/Game/World/World.h"

namespace LittleEngine
{
UIManager::UIManager()
{
	LOG_D("[UIManager] constructed");
}

UIManager::~UIManager()
{
	m_uContexts.clear();
	LOG_D("[UIManager] destroyed");
}

UIContext* UIManager::GetActiveContext() const
{
	return m_uContexts.empty() ? nullptr : m_uContexts.back().get();
}

void UIManager::Tick(Time dt)
{
	UIContext* pActive = GetActiveContext();
	if (pActive)
	{
		pActive->Tick(dt);
		if (pActive->m_bDestroyed)
		{
			m_uContexts.pop_back();
			if (!m_uContexts.empty())
				m_uContexts.back()->SetActive(true, false);
		}
	}
}
} // namespace LittleEngine
