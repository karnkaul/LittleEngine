#pragma once
#include "SFMLAPI/System/SFTypes.h"
#include "TRange.hpp"

namespace LittleEngine
{
template <typename T>
using TRange = Core::TRange<T>;
using SFPosition = TRange<Vector2>;
using SFOrientation = TRange<Fixed>;
using SFScale = TRange<Vector2>;
using SFColour = TRange<Colour>;

enum LayerID
{
	LAYER_ZERO = 0,
	LAYER_BACKGROUND = 10,
	LAYER_DEFAULT = 100,
	LAYER_LIVE = 200,
	LAYER_FX = 300,
	LAYER_UI = 400,
	LAYER_TOP = 490,
	LAYER_MAX = 499,
	_LAYER_COUNT = LAYER_MAX + 1,
};

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
