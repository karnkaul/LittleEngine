#include "SFCircle.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
SFCircle::SFCircle(LayerID layer) : ASFDrawable(layer) {}

SFCircle::~SFCircle() = default;

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

Rect2 SFCircle::GameBounds() const
{
	Fixed side(m_circleGameState.diameter * Fixed::OneHalf);
	Vector2 size(side, side);
	return Rect2::SizeCentre(size, m_gameState.tPosition.max);
}

void SFCircle::OnDraw(Viewport& viewport, sf::RenderStates& sfStates)
{
	viewport.draw(m_sfCircle, sfStates);
}

Vector2 SFCircle::SFSize() const
{
	Fixed cSize(m_sfCircle.getRadius() * 2);
	return {cSize, cSize};
}

sf::FloatRect SFCircle::SFBounds() const
{
	return m_sfCircle.getLocalBounds();
}

SFCircle* SFCircle::SetDiameter(Fixed diameter)
{
	m_circleGameState.diameter = diameter;
	SetDirty(false);
	return this;
}

Fixed SFCircle::Diameter() const
{
	return m_circleGameState.diameter;
}
} // namespace LittleEngine
