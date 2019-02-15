#include "stdafx.h"
#include "UIManager.h"
#include "LittleEngine/World/World.h"
#include "Logger.h"

namespace LittleEngine
{
UIManager::UIManager(World& owner) : UIObject("UIManager")
{
	m_baseClass = owner.m_name;
	LogD(LogName() + " constructed");
}

UIManager::~UIManager()
{
	m_uContexts.clear();
	LogD(LogName() + " destroyed");
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
