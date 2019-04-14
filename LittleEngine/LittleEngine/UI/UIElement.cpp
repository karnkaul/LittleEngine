#include "stdafx.h"
#include "Logger.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "UIElement.h"
#include "UIText.h"
#include "LittleEngine/Game/World/World.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/RenderLoop/RenderHeap.h"
#include "LittleEngine/Repository/EngineRepository.h"

namespace LittleEngine
{
UIElement::UIElement(bool bSilent) : UIObject("Untitled", bSilent)
{
	Construct();
}

UIElement::UIElement(String name, bool bSilent /*= false*/) : UIObject(std::move(name), bSilent)
{
	Construct();
}

UIElement::~UIElement()
{
	Services::RHeap()->Destroy(m_pPrimitive);
	Services::RHeap()->Destroy(m_pText);
}

void UIElement::SetParent(UITransform& parent)
{
	m_transform.SetParent(parent);
}

void UIElement::OnCreate(String name, UITransform* pParent)
{
	SetName(std::move(name), "UIElement");
	if (pParent)
	{
		m_transform.SetParent(*pParent);
	}
}

void UIElement::OnCreated()
{
}

void UIElement::Construct()
{
	m_pPrimitive = Services::RHeap()->New();
	m_pText = Services::RHeap()->New();
	m_pPrimitive->SetEnabled(true);
	m_pText->SetEnabled(true);
	m_pPrimitive->SetStatic(true);
	m_pText->SetStatic(true);
}

void UIElement::SetPanel(UByte r, UByte g, UByte b, UByte a)
{
	Colour colour(r, g, b, a);
	SetPanel(colour);
}

void UIElement::SetPanel(Colour fill, Fixed border, Colour outline)
{
	m_bPanel = true;
	m_pPrimitive->SetPrimaryColour(fill);
	m_pPrimitive->SetOutline(border);
	m_pPrimitive->SetSecondaryColour(outline);
	m_pPrimitive->SetSize(m_transform.size, SFShapeType::Rectangle);
}

void UIElement::SetImage(TextureAsset& texture, Colour colour)
{
	m_pPrimitive->SetTexture(texture);
	m_pPrimitive->SetPrimaryColour(colour);
}

void UIElement::SetText(UIText uiText)
{
	m_pText->SetFont(*Repository()->GetDefaultFont());
	m_pText->SetText(std::move(uiText.text));
	m_pText->SetPrimaryColour(uiText.colour);
	m_pText->SetTextSize(uiText.pixelSize);
}

void UIElement::SetFont(FontAsset& font)
{
	m_pText->SetFont(font);
}

SFPrimitive* UIElement::GetPrimitive() const
{
	return m_pPrimitive;
}

SFPrimitive* UIElement::GetText() const
{
	return m_pText;
}

void UIElement::Tick(Time)
{
	if (m_bDestroyed)
	{
		return;
	}
	if (m_transform.bAutoPad)
	{
		m_transform.SetAutoPadNPosition(m_transform.nPosition);
	}
	m_transform.anchor.x = Maths::Clamp_11(m_transform.anchor.x);
	m_transform.anchor.y = Maths::Clamp_11(m_transform.anchor.y);
	m_pPrimitive->SetPivot(m_transform.anchor);
	m_pText->SetPivot(m_transform.anchor);
	if (m_bPanel)
	{
		m_pPrimitive->SetSize(m_transform.size, SFShapeType::Rectangle);
	}
	m_pPrimitive->SetLayer(m_layer);
	m_pText->SetLayer(static_cast<LayerID>(m_layer + 1));
	m_pPrimitive->SetScale(Vector2::One, true);
	m_pText->SetScale(Vector2::One, true);
	m_pPrimitive->SetOrientation(Fixed::Zero, true);
	m_pText->SetOrientation(Fixed::Zero, true);
	m_pPrimitive->SetPosition(m_transform.GetWorldPosition());
	m_pText->SetPosition(m_transform.GetWorldPosition());
}
} // namespace LittleEngine
