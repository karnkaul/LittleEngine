#include "stdafx.h"
#include "Core/Logger.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/Rendering/Primitives.h"
#include "UIElement.h"
#include "UIText.h"
#include "LittleEngine/Renderer/LERenderer.h"
#include "LittleEngine/Repository/LERepository.h"
#include "LEGame/Model/GameManager.h"

namespace LittleEngine
{
UIElement::UIElement(LayerID layer, bool bSilent) : UIObject("Untitled", bSilent), m_layer(layer)
{
}

UIElement::~UIElement()
{
	m_pText->Destroy();
	m_pRect->Destroy();
}

void UIElement::SetParent(UITransform& parent)
{
	m_transform.SetParent(parent);
}

void UIElement::OnCreate(LEContext& context, String name, UITransform* pParent)
{
	SetContext(context);
	SetNameAndType(std::move(name), "UIElement");
	if (pParent)
	{
		m_transform.SetParent(*pParent);
	}
	Construct();
	OnCreated();
}

void UIElement::OnCreated()
{
}

void UIElement::Construct()
{
	m_transform.size = GetViewSize();
	m_pRect = Renderer()->New<SFRect>(m_layer);
	m_pText = Renderer()->New<SFText>(static_cast<LayerID>(m_layer + 1));
	m_pRect->SetStatic(true)->SetEnabled(true);
	m_pText->SetStatic(true)->SetEnabled(true);
}

void UIElement::SetPanel(UByte r, UByte g, UByte b, UByte a)
{
	Colour colour(r, g, b, a);
	SetPanel(colour);
}

void UIElement::SetPanel(Colour fill, Fixed border, Colour outline)
{
	m_bPanel = true;
	m_pRect->SetPrimaryColour(fill);
	m_pRect->SetOutline(border);
	m_pRect->SetSecondaryColour(outline);
	m_pRect->SetSize(m_transform.size);
}

//void UIElement::SetImage(TextureAsset& texture, Colour colour)
//{
//	m_pPrimitive->SetTexture(texture);
//	m_pPrimitive->SetPrimaryColour(colour);
//}

void UIElement::SetText(UIText uiText)
{
	m_pText->SetFont(*g_pRepository->GetDefaultFont());
	m_pText->SetText(std::move(uiText.text));
	m_pText->SetPrimaryColour(uiText.colour);
	m_pText->SetSize(uiText.pixelSize);
}

void UIElement::SetFont(FontAsset& font)
{
	m_pText->SetFont(font);
}

SFRect* UIElement::GetRect() const
{
	return m_pRect;
}

SFText* UIElement::GetText() const
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
	m_pRect->SetPivot(m_transform.anchor);
	m_pText->SetPivot(m_transform.anchor);
	if (m_bPanel)
	{
		m_pRect->SetSize(m_transform.size);
	}
	m_pRect->SetScale(Vector2::One, true);
	m_pText->SetScale(Vector2::One, true);
	m_pRect->SetOrientation(Fixed::Zero, true);
	m_pText->SetOrientation(Fixed::Zero, true);
	Vector2 viewSize = Renderer()->GetViewSize();
	m_pRect->SetPosition(m_transform.GetWorldPosition(viewSize));
	m_pText->SetPosition(m_transform.GetWorldPosition(viewSize));
}

LayerID UIElement::GetLayer() const
{
	return m_layer;
}
} // namespace LittleEngine
