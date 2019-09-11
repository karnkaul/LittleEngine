#include "SFRect.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
SFRect::SFRect(LayerID layer) : ASFDrawable(layer) {}

SFRect::~SFRect() = default;

Rect2 SFRect::GameBounds() const
{
	return Rect2::SizeCentre(m_rectGameState.tSize.max, m_gameState.tPosition.max);
}

void SFRect::SwapState()
{
	ASFDrawable::SwapState();
	m_rectRenderState = m_rectGameState;
}

void SFRect::ReconcileGameState()
{
	ASFDrawable::ReconcileGameState();
	m_rectGameState.tSize.min = m_rectGameState.tSize.max;
}

void SFRect::OnUpdateRenderState(Fixed alpha)
{
	Vector2 size = m_rectRenderState.tSize.Lerp(alpha);
	m_sfRect.setSize(Cast(size));

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

void SFRect::OnDraw(Viewport& viewport, sf::RenderStates& sfStates)
{
	viewport.draw(m_sfRect, sfStates);
}

Vector2 SFRect::SFSize() const
{
	return Cast(m_sfRect.getSize());
}

sf::FloatRect SFRect::SFBounds() const
{
	return m_sfRect.getLocalBounds();
}

SFRect* SFRect::SetSize(Vector2 size, bool bImmediate)
{
	if (bImmediate || !m_bRendered)
	{
		m_rectGameState.tSize.Reset(size);
	}
	else
	{
		m_rectGameState.tSize.Update(size);
	}
	SetDirty(false);
	return this;
}

Vector2 SFRect::Size() const
{
	return m_rectGameState.tSize.max;
}
} // namespace LittleEngine
