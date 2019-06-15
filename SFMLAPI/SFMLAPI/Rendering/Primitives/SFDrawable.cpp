#include "stdafx.h"
#include "Core/Logger.h"
#include "SFDrawable.h"
#include "SFMLAPI/System/SFTypes.h"

namespace LittleEngine
{
ASFDrawable::ASFDrawable(LayerID layer) : APrimitive(layer)
{
}

ASFDrawable::~ASFDrawable() = default;

ASFDrawable* ASFDrawable::SetOutline(Fixed outline)
{
	m_outline = outline;
	return this;
}

void ASFDrawable::OnUpdateRenderState(Fixed alpha)
{
	Vector2 offset = Fixed::OneHalf * GetBounds().GetSize();
	Vector2 pivot = Vector2(offset.x * m_renderState.pivot.x, offset.y * m_renderState.pivot.y);
	m_lerpedState.origin = Cast(WorldToScreen(pivot) + offset);
	m_lerpedState.scale = Cast(m_renderState.sfScale.Lerp(alpha));
	m_lerpedState.orientation = Cast(m_renderState.sfOrientation.Lerp(alpha));
	m_lerpedState.position = Cast(m_renderState.sfPosition.Lerp(alpha));
	m_lerpedState.primary =
		Cast(Colour::Lerp(m_renderState.sfPrimaryColour.min, m_renderState.sfPrimaryColour.max, alpha));
	m_lerpedState.secondary =
		Cast(Colour::Lerp(m_renderState.sfSecondaryColour.min, m_renderState.sfSecondaryColour.max, alpha));
	m_lerpedState.outline = Cast(m_outline);
}
}