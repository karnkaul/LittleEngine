#include "Core/Logger.h"
#include "SFMLAPI/System/Assets.h"
#include "SFMLAPI/Rendering/Primitives.h"
#include "Engine/GFX.h"
#include "Engine/Rendering/LERenderer.h"
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

void UIElement::OnCreate(LEContext& context, std::string name, UITransform* pParent, FontAsset* pFont)
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
	SetRectMaxSize();
	Regenerate(m_layer);
}

void UIElement::ApplyText()
{
	if (!m_pText->IsFontSet())
	{
		m_pText->SetFont(m_pFont ? *m_pFont : *g_pDefaultFont);
	}
	m_pText->SetColour(m_uiText.colour);
	m_pText->SetSize(m_uiText.pixelSize);
	m_pText->SetText(LOC(m_uiText.text));
}

void UIElement::ApplyPanel()
{
	m_bPanel = true;
	m_pRect->SetColour(m_uiPanel.fill);
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
	m_pText->SetColour(colour);
}

void UIElement::SetFont(FontAsset& font)
{
	m_pFont = &font;
	m_pText->SetFont(font);
}

void UIElement::SetRectSize(Vector2 size, bool bAutoPad /* = false */)
{
	m_transform.size = size;
	m_transform.bAutoPad = bAutoPad;
}

void UIElement::SetRectMaxSize()
{
	m_transform.size = g_pGFX->UISpace();
	m_transform.bAutoPad = false;
}

void UIElement::SetAutoPad(bool bAutoPad)
{
	m_transform.bAutoPad = bAutoPad;
}

Vector2 UIElement::RectSize() const
{
	return m_transform.size;
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
	Vector2 uiPos = m_transform.WorldPosition(g_pGFX->UISpace());
	m_pRect->SetPosition(uiPos, !m_bTicked);
	m_pText->SetPosition(uiPos, !m_bTicked);
	m_bDoTick = !m_bStopTicking;
	m_bTicked = true;
}

LayerID UIElement::GetLayer() const
{
	return m_layer;
}

void UIElement::Regenerate(LayerID newLayer)
{
	bool bRectStatic = true;
	bool bTextStatic = true;
	if (m_pRect)
	{
		bRectStatic = m_pRect->IsStatic();
		m_pRect->Destroy();
	}
	if (m_pText)
	{
		bTextStatic = m_pText->IsStatic();
		m_pText->Destroy();
	}
	m_layer = newLayer;
	auto pFont = m_pFont ? m_pFont : g_pRepository->Load<FontAsset>("Fonts/UIFont.ttf");
	m_pRect = Renderer()->New<SFRect>(m_layer);
	m_pText = Renderer()->New<SFText>(static_cast<LayerID>(ToS32(m_layer) + 1));
	m_pRect->SetStatic(bRectStatic)->SetEnabled(true);
	m_pText->SetFont(pFont ? *pFont : *g_pDefaultFont)->SetStatic(bTextStatic)->SetEnabled(true);
	ApplyText();
	if (m_bPanel)
	{
		ApplyPanel();
	}
	m_bTicked = false;
	Tick();
}

void UIElement::SetStatic(bool bStatic)
{
	m_bDoTick = true;
	m_bStopTicking = bStatic;
}
} // namespace LittleEngine
