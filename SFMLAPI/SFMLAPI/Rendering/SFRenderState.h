#pragma once
#include "Colour.h"
#include "TRange.hpp"
#include "SFLayerID.h"

namespace LittleEngine
{
template <typename T>
using TRange = Core::TRange<T>;
using SFPosition = TRange<Vector2>;
using SFOrientation = TRange<Fixed>;
using SFScale = TRange<Vector2>;
using SFColour = TRange<Colour>;

struct SFRenderState
{
	SFPosition sfPosition = Vector2::Zero;
	SFOrientation sfOrientation = Fixed::Zero;
	SFScale sfScale = Vector2::One;
	SFColour sfPrimaryColour = Colour(255, 255, 255, 255);
	SFColour sfSecondaryColour = Colour(255, 255, 255, 0);
	Vector2 pivot = Vector2::Zero;
	Fixed outlineThickness = Fixed::Zero;
	LayerID layer = LAYER_DEFAULT;

	bool bEnabled = false;

	void Reconcile();
};
} // namespace LittleEngine
