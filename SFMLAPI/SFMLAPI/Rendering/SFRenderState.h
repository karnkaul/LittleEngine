#pragma once
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "Core/TRange.h"
#include "Colour.h"
#include "SFLayerID.h"

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

// Origin at top-left of texture; +X right, +Y down
struct SFTexCoords
{
	u32 x = 0;
	u32 y = 0;

	SFTexCoords() = default;
	//+X right, +Y down
	SFTexCoords(u32 x, u32 y);

	static const SFTexCoords Zero;
	
	sf::Vector2f ToSFV2f() const;
};

// Origin at top-left of texture; +X right, +Y down
struct SFTexRect
{
	SFTexCoords min;
	SFTexCoords max;

	SFTexRect() = default;
	//+X right, +Y down
	SFTexRect(SFTexCoords min, SFTexCoords max);
	SFTexRect(u32 maxX, u32 maxY);

	static const SFTexRect Zero;

	sf::IntRect ToSFIntRect() const;
	bool IsZero() const;
};

struct SFRenderState
{
	SFPosition sfPosition = Vector2::Zero;
	SFOrientation sfOrientation = Fixed::Zero;
	SFScale sfScale = Vector2::One;
	SFColour sfPrimaryColour = Colour(255, 255, 255, 255);
	SFColour sfSecondaryColour = Colour(255, 255, 255, 0);
	Vector2 pivot = Vector2::Zero;
	String text;
	Fixed outlineThickness = Fixed::Zero;
	SFTexRect texRect = SFTexRect::Zero;
	Vector2 shapeSize = Vector2::Zero;
	u8 textSize = 0;
	const class FontAsset* pFont = nullptr;
	const class TextureAsset* pTexture = nullptr;
	LayerID layer = LAYER_DEFAULT;
	SFShapeType shape = SFShapeType::None;

	bool bEnabled = false;

	void Reconcile();
};
} // namespace LittleEngine
