#pragma once
#include "CoreTypes.h"

namespace LittleEngine
{
struct SFWindowSize
{
	u32 width;
	u32 height;

	SFWindowSize();
	SFWindowSize(u32 width, u32 height);
};

struct SFWindowData
{
	SFWindowSize windowSize;
	Vector2 viewSize;
	String title;

	SFWindowData();
	SFWindowData(const SFWindowSize& size, const Vector2& viewSize, const String& title);
};
} // namespace LittleEngine
