#pragma once
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "Core/CoreTypes.h"

namespace LittleEngine
{
// Origin at top-left of texture; +X right, +Y down
struct SFTexCoords
{
	u32 x = 0;
	u32 y = 0;

	SFTexCoords() = default;
	//+X right, +Y down
	SFTexCoords(u32 x, u32 y);
	explicit SFTexCoords(Vector2 point);

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

	void Import(const class TextureAsset& texture);

	static const SFTexRect Zero;

	sf::IntRect ToSFIntRect() const;
	bool IsZero() const;
};
} // namespace LittleEngine