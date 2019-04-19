#include "stdafx.h"
#include "SFRenderState.h"

namespace LittleEngine
{
const SFTexCoords SFTexCoords::Zero = SFTexCoords(0, 0);

SFTexCoords::SFTexCoords(u32 x, u32 y) : x(x), y(y)
{
}

const SFTexRect SFTexRect::Zero = SFTexRect(SFTexCoords::Zero, SFTexCoords::Zero);

SFTexRect::SFTexRect(SFTexCoords min, SFTexCoords max) : min(min), max(max)
{
}

sf::IntRect SFTexRect::Cast() const
{
	u32 x = max.x - min.x;
	u32 y = max.y - min.y;
	return sf::IntRect(min.x, min.y, x, y);
}

bool SFTexRect::IsZero() const
{
	return min.x == min.y == max.x == max.y == 0;
}

void SFRenderState::Reconcile()
{
	sfPosition.min = sfPosition.max;
	sfOrientation.min = sfOrientation.max;
	sfScale.min = sfScale.max;
	sfPrimaryColour.min = sfPrimaryColour.max;
	sfSecondaryColour.min = sfSecondaryColour.max;
}
} // namespace LittleEngine
