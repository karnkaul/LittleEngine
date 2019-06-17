#include "stdafx.h"
#include "SFCircle.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
SFCircle::SFCircle(LayerID layer) : ASFDrawable(layer)
{
}

SFCircle::~SFCircle() = default;

Rect2 SFCircle::GetBounds() const
{
	Fixed cSize(m_sfCircle.getRadius() * 2);
	Vector2 pivot = Cast(m_sfCircle.getOrigin());
	Vector2 size(cSize, cSize);
	Vector2 offset = Fixed::OneHalf * size;
	return Rect2::CentreSize(size, pivot + offset);
}

void SFCircle::SwapState()
{
	ASFDrawable::SwapState();
	m_circleRenderState = m_circleGameState;
}

void SFCircle::OnUpdateRenderState(Fixed alpha)
{
	m_sfCircle.setRadius(Cast(m_circleRenderState.diameter * Fixed::OneHalf));

	// sfCircle size needs to be set above before GetBounds() can be called here
	State s = GetState(alpha);
	DrawableState ds = GetDrawableState(alpha);
	m_sfCircle.setOrigin(Cast(ds.origin));
	m_sfCircle.setScale(Cast(s.scale));
	m_sfCircle.setRotation(Cast(Vector2::ToOrientation(s.orientation)));
	m_sfCircle.setPosition(Cast(s.position));
	m_sfCircle.setFillColor(Cast(s.colour));
	m_sfCircle.setOutlineThickness(Cast(ds.outline));
	m_sfCircle.setOutlineColor(Cast(ds.secondary));
}

void SFCircle::OnDraw(SFViewport& viewport, sf::RenderStates& sfStates)
{
	viewport.draw(m_sfCircle, sfStates);
}

SFCircle* SFCircle::SetDiameter(Fixed diameter)
{
	m_circleGameState.diameter = diameter;
	return this;
}
} // namespace LittleEngine