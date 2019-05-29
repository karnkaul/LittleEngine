#include "stdafx.h"
#include "Core/Logger.h"
#include "UIContext.h"
#include "UIManager.h"
#include "LEGame/GameModel/GameManager.h"

namespace LittleEngine
{
UIManager::UIManager(GameManager& gameManager) : m_pGameManager(&gameManager)
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
			{
				m_uContexts.back()->SetActive(true, false);
			}
		}
	}
}

void UIManager::InitContext(UIContext* pContext, String name, LayerID baseLayer)
{
	pContext->OnCreate(*m_pGameManager, std::move(name), baseLayer);
}
} // namespace LittleEngine
