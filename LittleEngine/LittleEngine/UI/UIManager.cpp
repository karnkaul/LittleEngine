#include "stdafx.h"
#include "UIManager.h"
#include "LittleEngine/World/World.h"
#include "Logger.h"

namespace LittleEngine
{
UIManager::UIManager(World& owner) : UIObject("UIManager")
{
	SetName("", owner.GetNameStr());
	LOG_D("%s constructed", LogNameStr());
}

UIManager::~UIManager()
{
	m_uContexts.clear();
	LOG_D("%s destroyed", LogNameStr());
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
