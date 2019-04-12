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
	: windowSize(WIDTH, HEIGHT), viewSize(WIDTH, HEIGHT), title("Untitled")
{
}
SFWindowData::SFWindowData(SFWindowSize windowSize, Vector2 viewSize, String title, SFWindowStyle style)
	: windowSize(std::move(windowSize)), viewSize(std::move(viewSize)), title(std::move(title)), style(style)
{
}

SFWindowRecreateData::SFWindowRecreateData(SFWindowSize size)
{
	windowSize.emplace(std::move(size));
}

SFWindowRecreateData::SFWindowRecreateData(SFWindowStyle style)
{
	this->style.emplace(style);
}

SFWindowRecreateData::SFWindowRecreateData(String title)
{
	this->title.emplace(std::move(title));
}
} // namespace LittleEngine
