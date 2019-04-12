#include "stdafx.h"
#include "SFWindowData.h"

namespace LittleEngine
{
namespace
{
constexpr u32 WIDTH = 1280;
constexpr u32 HEIGHT = 720;
} // namespace

SFWindowSize::SFWindowSize() = default;

SFWindowSize::SFWindowSize(u32 width, u32 height) : width(width), height(height)
{
}
SFWindowData::SFWindowData()
	: windowSize(WIDTH, HEIGHT), viewSize(WIDTH, HEIGHT), title("Untitled"), sfStyle(sf::Style::Default)
{
}
SFWindowData::SFWindowData(SFWindowSize windowSize, Vector2 viewSize, String title)
	: windowSize(std::move(windowSize)), viewSize(std::move(viewSize)), title(std::move(title)), sfStyle(sf::Style::Default)
{
}
} // namespace LittleEngine
