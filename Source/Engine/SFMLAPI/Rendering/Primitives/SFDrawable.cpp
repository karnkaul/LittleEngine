#include "Core/Logger.h"
#include "SFDrawable.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
void ASFDrawable::TDrawableState::Reconcile()
{
	tPivot.min = tPivot.max;
	tSecondaryColour.min = tSecondaryColour.max;
	tOutline.min = tOutline.max;
}

ASFDrawable::ASFDrawable(LayerID layer) : APrimitive(layer) {}

ASFDrawable::~ASFDrawable() = default;

ASFDrawable* ASFDrawable::SetPivot(Vector2 pivot, bool bImmediate)
{
	if (bImmediate)
	{
		m_drawableGameState.tPivot.Reset(WorldToSFML(pivot));
	}
	else
	{
		m_drawableGameState.tPivot.Update(WorldToSFML(pivot));
	}
	SetDirty(true);
	return this;
}

ASFDrawable* ASFDrawable::SetOutline(Fixed outline, bool bImmediate)
{
	if (bImmediate)
	{
		m_drawableGameState.tOutline.Reset(outline);
	}
	else
	{
		m_drawableGameState.tOutline.Update(outline);
	}
	SetDirty(true);
	return this;
}

ASFDrawable* ASFDrawable::SetSecondaryColour(Colour colour, bool bImmediate)
{
	if (bImmediate)
	{
		m_drawableGameState.tSecondaryColour.Reset(colour);
	}
	else
	{
		m_drawableGameState.tSecondaryColour.Update(colour);
	}
	SetDirty(true);
	return this;
}

Vector2 ASFDrawable::Pivot() const
{
	return m_drawableGameState.tPivot.max;
}

Fixed ASFDrawable::Outline() const
{
	return m_drawableGameState.tOutline.max;
}

Colour ASFDrawable::SecondaryColour() const
{
	return m_drawableGameState.tSecondaryColour.max;
}

Rect2 ASFDrawable::RenderBounds() const
{
	sf::FloatRect bounds = SFBounds();
	Vector2 size(Fixed(bounds.width), Fixed(bounds.height));
	Vector2 centre = SFMLToWorld(m_renderState.tPosition.max);
	return Rect2::SizeCentre(size, centre);
}

void ASFDrawable::ReconcileGameState()
{
	APrimitive::ReconcileGameState();
	m_drawableGameState.Reconcile();
}

void ASFDrawable::SwapState()
{
	APrimitive::SwapState();
	m_drawableRenderState = m_drawableGameState;
}

ASFDrawable::DrawableState ASFDrawable::GetDrawableState(Fixed alpha) const
{
	DrawableState ret;
	Vector2 offset = Fixed::OneHalf * SFSize();
	Vector2 pivot = m_drawableRenderState.tPivot.Lerp(alpha);
	ret.origin = Vector2(offset.x * pivot.x, offset.y * pivot.y) + offset;
	ret.secondary = Colour::Lerp(m_drawableRenderState.tSecondaryColour, alpha);
	ret.outline = m_drawableRenderState.tOutline.Lerp(alpha);
	return ret;
}
} // namespace LittleEngine
