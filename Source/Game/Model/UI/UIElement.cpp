#include "Core/Logger.h"
#include "SFMLAPI/System/Assets.h"
#include "SFMLAPI/Rendering/Primitives.h"
#include "Engine/Renderer/LERenderer.h"
#include "Engine/Repository/LERepository.h"
#include "Engine/Locale/Locale.h"
#include "UIElement.h"
#include "UIText.h"
#include "Model/GameManager.h"

namespace LittleEngine
{
UIElement::UIElement(LayerID layer, bool bSilent) : UIObject("Untitled", bSilent), m_layer(layer) {}

UIElement::~UIElement()
{
	m_pText->Destroy();
	m_pRect->Destroy();
}

void UIElement::SetParent(UITransform& parent)
{
	m_transform.SetParent(parent);
}

void UIElement::OnCreate(LEContext& context, String name, UITransform* pParent, FontAsset* pFont)
{
	m_pFont = pFont;
	SetContext(context);
	SetNameAndType(std::move(name), "UIElement");
	if (pParent)
	{
		m_transform.SetParent(*pParent);
	}
	Construct();
	OnCreated();
}

void UIElement::OnCreated() {}

void UIElement::Construct()
{
	m_transform.size = ViewSize();
	Regenerate(m_layer);
}

void UIElement::ApplyText()
{
	if (!m_pText->IsFontSet())
	{
		m_pText->SetFont(m_pFont ? *m_pFont : *g_pRepository->DefaultFont());
	}
	m_pText->SetPrimaryColour(m_uiText.colour);
	m_pText->SetSize(m_uiText.pixelSize);
	m_pText->SetText(LOC(m_uiText.text));
}

void UIElement::ApplyPanel()
{
	m_bPanel = true;
	m_pRect->SetPrimaryColour(m_uiPanel.fill);
	m_pRect->SetOutline(m_uiPanel.border);
	m_pRect->SetSecondaryColour(m_uiPanel.outline);
	m_pRect->SetSize(m_transform.size, !m_bLerpSize);
}

void UIElement::SetPanel(UByte r, UByte g, UByte b, UByte a)
{
	Colour colour(r, g, b, a);
	SetPanel(colour);
}

void UIElement::SetPanel(Colour fill, Fixed border, Colour outline)
{
	m_uiPanel.border = border;
	m_uiPanel.fill = fill;
	m_uiPanel.outline = outline;
	ApplyPanel();
}

// void UIElement::SetImage(TextureAsset& texture, Colour colour)
//{
//	m_pPrimitive->SetTexture(texture);
//	m_pPrimitive->SetPrimaryColour(colour);
//}

void UIElement::SetText(UIText uiText)
{
	m_uiText = std::move(uiText);
	ApplyText();
}

void UIElement::SetTextColour(Colour colour)
{
	m_uiText.colour = colour;
	m_pText->SetPrimaryColour(colour);
}

void UIElement::SetFont(FontAsset& font)
{
	m_pFont = &font;
	m_pText->SetFont(font);
}

SFRect* UIElement::Rect() const
{
	return m_pRect;
}

SFText* UIElement::Text() const
{
	return m_pText;
}

void UIElement::SetEnabled(bool bEnabled)
{
	if (m_pRect)
	{
		m_pRect->SetEnabled(bEnabled);
	}
	if (m_pText)
	{
		m_pText->SetEnabled(bEnabled);
	}
}

void UIElement::Tick(Time /*dt*/)
{
	if (m_bDestroyed || !m_bDoTick)
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
		m_pRect->SetSize(m_transform.size, !m_bLerpSize);
	}
	m_pRect->SetScale(Vector2::One, true);
	m_pText->SetScale(Vector2::One, true);
	m_pRect->SetOrientation(Vector2::Right, true);
	m_pText->SetOrientation(Vector2::Right, true);
	Vector2 viewSize = Renderer()->ViewSize();
	m_pRect->SetPosition(m_transform.WorldPosition(viewSize));
	m_pText->SetPosition(m_transform.WorldPosition(viewSize));

	m_bDoTick = !m_bStopTicking;
}

LayerID UIElement::GetLayer() const
{
	return m_layer;
}

void UIElement::Regenerate(LayerID newLayer)
{
	if (m_pRect)
	{
		m_pRect->Destroy();
	}
	if (m_pText)
	{
		m_pText->Destroy();
	}
	m_layer = newLayer;
	auto pFont = m_pFont ? m_pFont : g_pRepository->Load<FontAsset>("Fonts/UIFont.ttf");
	m_pRect = Renderer()->New<SFRect>(m_layer);
	m_pText = Renderer()->New<SFText>(static_cast<LayerID>(m_layer + 1));
	m_pRect->SetStatic(true)->SetEnabled(true);
	m_pText->SetFont(pFont ? *pFont : *g_pRepository->DefaultFont())->SetStatic(true)->SetEnabled(true);
	ApplyText();
	if (m_bPanel)
	{
		ApplyPanel();
	}
}

void UIElement::SetStatic(bool bStatic)
{
	m_bDoTick = true;
	m_bStopTicking = bStatic;
}
} // namespace LittleEngine
