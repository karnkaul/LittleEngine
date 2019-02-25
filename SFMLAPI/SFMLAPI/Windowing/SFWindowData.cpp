#include "stdafx.h"
#include "SFWindowData.h"

namespace LittleEngine
{
namespace
{
constexpr u32 WIDTH = 1280;
constexpr u32 HEIGHT = 720;
} // namespace

SFWindowSize::SFWindowSize() : width(0), height(0)
{
}
SFWindowSize::SFWindowSize(u32 width, u32 height) : width(width), height(height)
{
}
SFWindowData::SFWindowData() : windowSize(WIDTH, HEIGHT), viewSize(WIDTH, HEIGHT), title("Untitled")
{
}
SFWindowData::SFWindowData(const SFWindowSize& windowSize, const Vector2& viewSize, const String& title)
	: windowSize(windowSize), viewSize(viewSize), title(title)
{
}
} // namespace LittleEngine
