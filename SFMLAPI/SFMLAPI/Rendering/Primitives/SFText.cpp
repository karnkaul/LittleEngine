#include "stdafx.h"
#include "Core/Logger.h"
#include "SFText.h"
#include "SFMLAPI/System/SFAssets.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
SFText::SFText(LayerID layer) : ASFDrawable(layer)
{
}

SFText::~SFText() = default;

Rect2 SFText::GetBounds(bool bWorld) const
{
	sf::FloatRect bounds = m_sfText.getLocalBounds();
	Vector2 size(Fixed(bounds.width), Fixed(bounds.height));
	Vector2 centre = bWorld ? ScreenToWorld(m_gameState.tPosition.max) : Vector2::Zero;
	return Rect2::CentreSize(size, centre);
}

void SFText::SwapState()
{
	ASFDrawable::SwapState();
	m_textRenderState = m_textGameState;
}

void SFText::OnUpdateRenderState(Fixed alpha)
{
	if (m_bTextChanged.load(std::memory_order_relaxed))
	{
		if (m_pFont)
		{
			m_sfText.setFont(m_pFont->m_sfFont);
		}
		m_sfText.setCharacterSize(m_textRenderState.size);
		m_sfText.setString(m_textRenderState.text);
		m_bTextChanged.store(false, std::memory_order_relaxed);
	}

	// sfText needs to be set above before GetBounds() can be called here
	State s = GetState(alpha);
	DrawableState ds = GetDrawableState(alpha);
	m_sfText.setOrigin(Cast(ds.origin));
	m_sfText.setScale(Cast(s.scale));
	m_sfText.setRotation(Cast(Vector2::ToOrientation(s.orientation)));
	m_sfText.setPosition(Cast(s.position));
	m_sfText.setFillColor(Cast(s.colour));
	m_sfText.setOutlineThickness(Cast(ds.outline));
	m_sfText.setOutlineColor(Cast(ds.secondary));
	m_sfText.setCharacterSize(m_textRenderState.size);
}

void SFText::OnDraw(SFViewport& viewport, sf::RenderStates& states)
{
	viewport.draw(m_sfText, states);
}

SFText* SFText::SetFont(FontAsset& font)
{
	m_pFont = &font;
	m_bTextChanged.store(true, std::memory_order_relaxed);
	return this;
}

SFText* SFText::SetSize(u32 size)
{
	m_textGameState.size = size;
	m_bTextChanged.store(true, std::memory_order_relaxed);
	return this;
}

SFText* SFText::SetText(String text)
{
	m_textGameState.text = std::move(text);
	m_bTextChanged.store(true, std::memory_order_relaxed);
	return this;
}
} // namespace LittleEngine