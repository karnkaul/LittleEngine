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
	if (m_uActiveContext)
		m_uActiveContext = nullptr;
	LogD(LogName() + " destroyed");
}

UIContext* UIManager::GetActiveContext() const
{
	return m_uActiveContext.get();
}

void UIManager::Tick(Time dt)
{
	if (m_uActiveContext)
	{
		m_uActiveContext->Tick(dt);
		if (m_uActiveContext->m_bDestroyed)
			m_uActiveContext = nullptr;
	}
}
} // namespace LittleEngine
