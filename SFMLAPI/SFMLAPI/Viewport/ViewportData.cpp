#include "stdafx.h"
#include "ViewportData.h"

namespace LittleEngine
{
namespace
{
constexpr u32 WIDTH = 1280;
constexpr u32 HEIGHT = 720;
} // namespace

ViewportData::ViewportData()
	: viewportSize{WIDTH, HEIGHT}, viewSize(WIDTH, HEIGHT), title("Untitled")
{
}
ViewportData::ViewportData(ViewportSize viewportSize, Vector2 viewSize, String title, ViewportStyle style)
	: viewportSize(std::move(viewportSize)), viewSize(std::move(viewSize)), title(std::move(title)), style(style)
{
}

ViewportRecreateData::ViewportRecreateData(ViewportSize size)
{
	oViewportSize.emplace(std::move(size));
}

ViewportRecreateData::ViewportRecreateData(ViewportStyle style)
{
	this->oSstyle.emplace(style);
}

ViewportRecreateData::ViewportRecreateData(String title)
{
	this->oTitle.emplace(std::move(title));
}
} // namespace LittleEngine
