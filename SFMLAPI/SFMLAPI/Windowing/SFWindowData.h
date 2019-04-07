#pragma once
#include "CoreTypes.h"
#include "SFML/Config.hpp"

namespace sf
{
typedef unsigned char Uint8;
}

namespace LittleEngine
{
struct SFWindowSize
{
	u32 width = 0;
	u32 height = 0;

	SFWindowSize();
	SFWindowSize(u32 width, u32 height);
	SFWindowSize(SFWindowSize&&) = default;
	SFWindowSize& operator=(SFWindowSize&&) = default;
};

struct SFWindowData
{
	SFWindowSize windowSize;
	Vector2 viewSize;
	String title;
	sf::Uint8 sfStyle;

	SFWindowData();
	SFWindowData(SFWindowSize size, Vector2 viewSize, String title);
};
} // namespace LittleEngine
