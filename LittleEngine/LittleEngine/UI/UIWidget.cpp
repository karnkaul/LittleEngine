#include "stdafx.h"
#include "UIWidget.h"
#include "LittleEngine/UI/UIContext.h"
#include "Logger.h"

namespace LittleEngine
{
UIWidget::UIWidget() : UIObject("Untitled")
{
	LogD("Untitled UIWidget constructed");
}

UIWidget::UIWidget(const String& name) : UIObject(name)
{
	LogD(LogName() + " UIWidget constructed");
}

UIWidget::~UIWidget()
{
	LogD(LogName() + " UIWidget destroyed");
}

void UIWidget::InitWidget(UIContext& owner)
{
	this->m_pOwner = &owner;
}

void UIWidget::Tick(Time dt)
{
	if (m_bDestroyed)
		return;
	for (auto& uUIelement : m_uiElements)
	{
		uUIelement->Tick(dt);
	}
}

void UIWidget::InitElement(UIElement* pNewElement)
{
	UITransform* pParent = m_pOwner ? &m_pOwner->GetRootElement()->m_transform : nullptr;
	pNewElement->InitElement(pParent);
}
} // namespace LittleEngine
