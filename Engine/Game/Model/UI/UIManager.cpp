#include "UIManager.h"
#include "Game/Model/GameManager.h"

namespace LittleEngine
{
UIManager::UIManager()
{
	LOG_D("[UIManager] constructed");
}

UIManager::~UIManager()
{
	Clear();
	m_inactive.clear();
	LOG_D("[UIManager] destroyed");
}

UIContext* UIManager::Active() const
{
	return m_contexts.empty() ? nullptr : m_contexts.back().get();
}

void UIManager::Clear()
{
	for (auto& uContext : m_contexts)
	{
		if (uContext->IsPersistent())
		{
			DisableContext(*uContext);
			m_inactive[uContext->m_name] = std::move(uContext);
		}
	}
	m_contexts.clear();
}

void UIManager::Tick(Time dt)
{
	UIContext* pActive = Active();
	if (pActive)
	{
		if (pActive->m_bDestroyed)
		{
			if (pActive->IsPersistent())
			{
				DisableContext(*pActive);
				m_inactive[pActive->m_name] = std::move(m_contexts.back());
			}
			m_contexts.pop_back();
			if (!m_contexts.empty())
			{
				auto& uContext = m_contexts.back();
				uContext->SetActive(true);
			}
		}
	}

	auto iter = m_contexts.begin();
	while (iter != m_contexts.end())
	{
		UIContext* pContext = iter->get();
		if (pContext->m_bDestroyed)
		{
			if (pContext->IsPersistent())
			{
				DisableContext(*pContext);
				m_inactive[pContext->m_name] = std::move(*iter);
			}
			iter = m_contexts.erase(iter);
		}
		else
		{
			pContext->Tick(dt);
			++iter;
		}
	}
}

void UIManager::InitContext(UIContext& context, std::string id, LayerID baseLayer)
{
	context.OnCreate(std::move(id), baseLayer);
}

void UIManager::DisableContext(UIContext& context)
{
	context.SetActive(false);
	context.SetEnabled(false);
}
} // namespace LittleEngine
