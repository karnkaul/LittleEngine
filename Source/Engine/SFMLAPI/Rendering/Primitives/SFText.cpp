#include "Core/Logger.h"
#include "SFText.h"
#include "SFMLAPI/System/Assets.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
SFText::SFText(LayerID layer) : ASFDrawable(layer) {}

SFText::~SFText() = default;

Rect2 SFText::GameBounds() const
{
	return RenderBounds();
}

void SFText::OnUpdateRenderState(Fixed alpha)
{
	Lock lock(m_textMutex);
	UpdateSFText();
	State s = GetState(alpha);
	DrawableState ds = GetDrawableState(alpha);
	m_sfText.setOrigin(Cast(ds.origin));
	m_sfText.setScale(Cast(s.scale));
	m_sfText.setRotation(Cast(Vector2::ToOrientation(s.orientation)));
	m_sfText.setPosition(Cast(s.position));
	m_sfText.setFillColor(Cast(s.colour));
	m_sfText.setOutlineThickness(Cast(ds.outline));
	m_sfText.setOutlineColor(Cast(ds.secondary));
	m_sfText.setCharacterSize(m_textSize);
}

void SFText::OnDraw(Viewport& viewport, sf::RenderStates& states)
{
	Lock lock(m_textMutex);
	viewport.draw(m_sfText, states);
}

Vector2 SFText::SFSize() const
{
	sf::FloatRect bounds = SFBounds();
	return {Fixed(bounds.width), Fixed(bounds.height)};
}

sf::FloatRect SFText::SFBounds() const
{
	return m_text.empty() ? sf::FloatRect() : m_sfText.getLocalBounds();
}

SFText* SFText::SetFont(FontAsset& font)
{
	Assert(&font, "Font is null!");
	m_pFont = &font;
	SetDirty(false);
	return this;
}

SFText* SFText::SetSize(u32 size)
{
	m_textSize = size;
	SetDirty(false);
	return this;
}

SFText* SFText::SetText(String text)
{
	Lock lock(m_textMutex);
	m_text = std::move(text);
	UpdateSFText();
	return this;
}

String SFText::Text() const
{
	return m_text;
}

u32 SFText::TextSize() const
{
	return m_textSize;
}

bool SFText::IsFontSet() const
{
	return m_pFont != nullptr;
}

void SFText::UpdateSFText()
{
	if (m_pFont)
	{
		m_sfText.setFont(m_pFont->m_sfFont);
	}
	m_sfText.setString(m_text);
	m_sfText.setCharacterSize(m_textSize);
}
} // namespace LittleEngine
