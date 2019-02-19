#include "stdafx.h"
#include "UIWidget.h"
#include "LittleEngine/UI/UIContext.h"
#include "Logger.h"

namespace LittleEngine
{
UIWidget::UIWidget() : UIObject("Untitled")
{
	SetName("", "UIWidget");
}

UIWidget::UIWidget(const String& name) : UIObject(name)
{
	SetName("", "UIWidget");
}

UIWidget::~UIWidget()
{
	LOG_D("%s destroyed", LogNameStr());
}

void UIWidget::InitWidget(UIContext& owner, LayerID rootLayer)
{
	this->m_pOwner = &owner;
	m_rootLayer = rootLayer;
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

void UIWidget::InitElement(UIElement* pNewElement, UITransform* pParent)
{
	if (!pParent)
		pParent = m_pOwner ? &m_pOwner->GetRootElement()->m_transform : nullptr;
	pNewElement->InitElement(pParent);
	pNewElement->m_layer = m_rootLayer;
}
} // namespace LittleEngine
