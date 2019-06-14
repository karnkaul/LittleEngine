#include "stdafx.h"
#include "SFRect.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
SFRect::SFRect(LayerID layer) : ASFDrawable(layer)
{
}

SFRect::~SFRect() = default;

void SFRect::OnUpdateRenderState(Fixed alpha)
{
	m_sfRect.setSize(Cast(m_rectRenderState.size));
	
	// sfRect size needs to be set above before GetBounds() can be called here
	ASFDrawable::OnUpdateRenderState(alpha);

	m_sfRect.setOrigin(m_lerpedState.origin);
	m_sfRect.setScale(m_lerpedState.scale);
	m_sfRect.setRotation(m_lerpedState.orientation);
	m_sfRect.setPosition(m_lerpedState.position);
	m_sfRect.setFillColor(m_lerpedState.primary);
	m_sfRect.setOutlineThickness(Cast(m_outline));
	m_sfRect.setOutlineColor(m_lerpedState.secondary);
}

void SFRect::OnDraw(SFViewport& viewport, sf::RenderStates& sfStates)
{
	viewport.draw(m_sfRect, sfStates);
}

void SFRect::OnSwapState()
{
	m_rectRenderState = m_rectGameState;
}

Rect2 SFRect::GetBounds() const
{
	Vector2 pivot = Cast(m_sfRect.getOrigin());
	Vector2 size = Cast(m_sfRect.getSize());
	Vector2 offset = Fixed::OneHalf * size;
	return Rect2::CentreSize(size, pivot + offset);
}

SFRect* SFRect::SetSize(Vector2 size)
{
	m_rectGameState.size = size;
	return this;
}
}