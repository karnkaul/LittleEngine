#include "stdafx.h"
#include "SFTexRect.h"
#include "SFMLAPI/System/SFAssets.h"

namespace LittleEngine
{
const SFTexCoords SFTexCoords::Zero = SFTexCoords(0, 0);

SFTexCoords::SFTexCoords(u32 x, u32 y) : x(x), y(y)
{
}

SFTexCoords::SFTexCoords(Vector2 point) : x(point.x.ToU32()), y(point.y.ToU32())
{
}

sf::Vector2f SFTexCoords::ToSFV2f() const
{
	return sf::Vector2f(x, y);
}

const SFTexRect SFTexRect::Zero = SFTexRect(SFTexCoords::Zero, SFTexCoords::Zero);

SFTexRect::SFTexRect(SFTexCoords min, SFTexCoords max) : min(min), max(max)
{
}

SFTexRect::SFTexRect(u32 maxX, u32 maxY) : min(SFTexCoords::Zero), max(SFTexCoords(maxX, maxY))
{
}

void SFTexRect::Import(const TextureAsset& texture)
{
	Vector2 size = texture.GetTextureSize();
	*this = SFTexRect(size.x.ToU32(), size.y.ToU32());
}

sf::IntRect SFTexRect::ToSFIntRect() const
{
	u32 x = max.x - min.x;
	u32 y = max.y - min.y;
	return sf::IntRect(min.x, min.y, x, y);
}

bool SFTexRect::IsZero() const
{
	return min.x == min.y == max.x == max.y == 0;
}
} // namespace LittleEngine