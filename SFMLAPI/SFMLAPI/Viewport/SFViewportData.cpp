#include "stdafx.h"
#include "SFViewportData.h"

namespace LittleEngine
{
namespace
{
constexpr u32 WIDTH = 1280;
constexpr u32 HEIGHT = 720;
} // namespace

SFViewportData::SFViewportData()
	: viewportSize{WIDTH, HEIGHT}, viewSize(WIDTH, HEIGHT), title("Untitled")
{
}
SFViewportData::SFViewportData(SFViewportSize viewportSize, Vector2 viewSize, String title, SFViewportStyle style)
	: viewportSize(std::move(viewportSize)), viewSize(std::move(viewSize)), title(std::move(title)), style(style)
{
}

SFViewportRecreateData::SFViewportRecreateData(SFViewportSize size)
{
	oViewportSize.emplace(std::move(size));
}

SFViewportRecreateData::SFViewportRecreateData(SFViewportStyle style)
{
	this->oSstyle.emplace(style);
}

SFViewportRecreateData::SFViewportRecreateData(String title)
{
	this->oTitle.emplace(std::move(title));
}
} // namespace LittleEngine
