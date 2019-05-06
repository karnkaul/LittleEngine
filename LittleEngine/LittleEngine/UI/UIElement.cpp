#include "stdafx.h"
#include "Core/Logger.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "UIElement.h"
#include "UIText.h"
#include "LittleEngine/Game/World/World.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/RenderLoop/RenderFactory.h"
#include "LittleEngine/Repository/EngineRepository.h"

namespace LittleEngine
{
UIElement::UIElement(LayerID layer, bool bSilent) : UIObject("Untitled", bSilent)
{
	Construct(layer);
}

UIElement::UIElement(String name, LayerID layer, bool bSilent /*= false*/) : UIObject(std::move(name), bSilent)
{
	Construct(layer);
}

UIElement::~UIElement()
{
	m_pPrimitive->Destroy();
	m_pText->Destroy();
}

void UIElement::SetParent(UITransform& parent)
{
	m_transform.SetParent(parent);
}

void UIElement::OnCreate(String name, UITransform* pParent)
{
	SetNameAndType(std::move(name), "UIElement");
	if (pParent)
	{
		m_transform.SetParent(*pParent);
	}
}

void UIElement::OnCreated()
{
}

void UIElement::Construct(LayerID layer)
{
	m_layer = layer;
	m_pPrimitive = Services::RFactory()->New(layer);
	m_pText = Services::RFactory()->New(static_cast<LayerID>(layer + 1));
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
	m_pPrimitive->SetScale(Vector2::One, true);
	m_pText->SetScale(Vector2::One, true);
	m_pPrimitive->SetOrientation(Fixed::Zero, true);
	m_pText->SetOrientation(Fixed::Zero, true);
	m_pPrimitive->SetPosition(m_transform.GetWorldPosition());
	m_pText->SetPosition(m_transform.GetWorldPosition());
}

LayerID UIElement::GetLayer() const
{
	return m_layer;
}
} // namespace LittleEngine
