#pragma once
#include "Core/TRange.h"
#include "Colour.h"
#include "SFLayerID.h"
#include "SFTexRect.h"

namespace LittleEngine
{
template <typename T>
using TRange = Core::TRange<T>;
using SFPosition = TRange<Vector2>;
using SFOrientation = TRange<Fixed>;
using SFScale = TRange<Vector2>;
using SFColour = TRange<Colour>;

enum class SFShapeType
{
	None,
	Rectangle,
	Circle,
};

struct SFRenderState
{
	SFPosition sfPosition = Vector2::Zero;
	SFOrientation sfOrientation = Fixed::Zero;
	SFScale sfScale = Vector2::One;
	SFColour sfPrimaryColour = Colour(255, 255, 255, 255);
	SFColour sfSecondaryColour = Colour(255, 255, 255, 0);

	SFTexRect texRect = SFTexRect::Zero;
	Vector2 pivot = Vector2::Zero;
	Vector2 shapeSize = Vector2::Zero;
	String text;
	Fixed outlineThickness = Fixed::Zero;
	u8 textSize = 0;
	const class FontAsset* pFont = nullptr;
	const class TextureAsset* pTexture = nullptr;
	SFShapeType shape = SFShapeType::None;

	bool bEnabled = false;

	inline void Reconcile()
	{
		sfPosition.min = sfPosition.max;
		sfOrientation.min = sfOrientation.max;
		sfScale.min = sfScale.max;
		sfPrimaryColour.min = sfPrimaryColour.max;
		sfSecondaryColour.min = sfSecondaryColour.max;
	}
};
} // namespace LittleEngine
