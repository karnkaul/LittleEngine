#include "stdafx.h"
#include "SFRenderState.h"

namespace LittleEngine
{
void SFRenderState::Reconcile()
{
	sfPosition.min = sfPosition.max;
	sfOrientation.min = sfOrientation.max;
	sfScale.min = sfScale.max;
	sfPrimaryColour.min = sfPrimaryColour.max;
	sfSecondaryColour.min = sfSecondaryColour.max;
}
} // namespace LittleEngine
