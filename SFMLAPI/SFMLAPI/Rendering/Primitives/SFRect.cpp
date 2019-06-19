#include "stdafx.h"
#include "SFRect.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
SFRect::SFRect(LayerID layer) : ASFDrawable(layer)
{
}

SFRect::~SFRect() = default;

Rect2 SFRect::GetBounds(bool bWorld) const
{
	Vector2 size = Cast(m_sfRect.getSize());
	Vector2 centre = bWorld ? ScreenToWorld(m_gameState.tPosition.max) : Vector2::Zero;
	return Rect2::CentreSize(size, centre);
}

void SFRect::SwapState()
{
	ASFDrawable::SwapState();
	m_rectRenderState = m_rectGameState;
}

void SFRect::OnUpdateRenderState(Fixed alpha)
{
	m_sfRect.setSize(Cast(m_rectRenderState.size));

	// sfRect size needs to be set above before GetBounds() can be called here
	State s = GetState(alpha);
	DrawableState ds = GetDrawableState(alpha);
	m_sfRect.setOrigin(Cast(ds.origin));
	m_sfRect.setScale(Cast(s.scale));
	m_sfRect.setRotation(Cast(Vector2::ToOrientation(s.orientation)));
	m_sfRect.setPosition(Cast(s.position));
	m_sfRect.setFillColor(Cast(s.colour));
	m_sfRect.setOutlineThickness(Cast(ds.outline));
	m_sfRect.setOutlineColor(Cast(ds.secondary));
}

void SFRect::OnDraw(SFViewport& viewport, sf::RenderStates& sfStates)
{
	viewport.draw(m_sfRect, sfStates);
}

SFRect* SFRect::SetSize(Vector2 size)
{
	m_rectGameState.size = size;
	return this;
}
} // namespace LittleEngine