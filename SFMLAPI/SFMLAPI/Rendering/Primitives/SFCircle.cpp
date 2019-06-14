#include "stdafx.h"
#include "SFCircle.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
SFCircle::SFCircle(LayerID layer) : ASFDrawable(layer)
{
}

SFCircle::~SFCircle() = default;

void SFCircle::OnUpdateRenderState(Fixed alpha)
{
	m_sfCircle.setRadius(Cast(m_circleRenderState.diameter * Fixed::OneHalf));

	// sfCircle size needs to be set above before GetBounds() can be called here
	ASFDrawable::OnUpdateRenderState(alpha);

	m_sfCircle.setOrigin(m_lerpedState.origin);
	m_sfCircle.setScale(m_lerpedState.scale);
	m_sfCircle.setRotation(m_lerpedState.orientation);
	m_sfCircle.setPosition(m_lerpedState.position);
	m_sfCircle.setFillColor(m_lerpedState.primary);
	m_sfCircle.setOutlineThickness(Cast(m_outline));
	m_sfCircle.setOutlineColor(m_lerpedState.secondary);
}

void SFCircle::OnDraw(SFViewport& viewport, sf::RenderStates& sfStates)
{
	viewport.draw(m_sfCircle, sfStates);
}

void SFCircle::OnSwapState()
{
	m_circleRenderState = m_circleGameState;
}

Rect2 SFCircle::GetBounds() const
{
	Fixed cSize(m_sfCircle.getRadius() * 2);
	Vector2 pivot = Cast(m_sfCircle.getOrigin());
	Vector2 size(cSize, cSize);
	Vector2 offset = Fixed::OneHalf * size;
	return Rect2::CentreSize(size, pivot + offset);
}

SFCircle* SFCircle::SetDiameter(Fixed diameter)
{
	m_circleGameState.diameter = diameter;
	return this;
}
}