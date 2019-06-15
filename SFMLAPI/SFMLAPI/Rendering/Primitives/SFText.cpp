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

Rect2 SFText::GetBounds() const
{
	sf::FloatRect bounds = m_sfText.getLocalBounds();
	Vector2 size(Fixed(bounds.width), Fixed(bounds.height));
	Vector2 offset = Fixed::OneHalf * size;
	Vector2 pivot = Cast(m_sfText.getOrigin());
	return Rect2::CentreSize(size, pivot + offset);
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
		m_sfText.setOutlineThickness(Cast(m_outline));
		m_bTextChanged.store(false, std::memory_order_relaxed);
	}

	// sfText needs to be set above before GetBounds() can be called here
	ASFDrawable::OnUpdateRenderState(alpha);

	m_sfText.setOrigin(m_lerpedState.origin);
	m_sfText.setScale(m_lerpedState.scale);
	m_sfText.setRotation(m_lerpedState.orientation);
	m_sfText.setPosition(m_lerpedState.position);
	m_sfText.setFillColor(m_lerpedState.primary);
	m_sfText.setOutlineThickness(Cast(m_outline));
	m_sfText.setOutlineColor(m_lerpedState.secondary);
	m_sfText.setCharacterSize(m_textRenderState.size);
}

void SFText::OnDraw(SFViewport& viewport, sf::RenderStates& states)
{
	viewport.draw(m_sfText, states);
}

void SFText::OnSwapState()
{
	m_textRenderState = m_textGameState;
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