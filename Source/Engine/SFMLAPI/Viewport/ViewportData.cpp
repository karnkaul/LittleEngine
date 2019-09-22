#include "ViewportData.h"

namespace LittleEngine
{
namespace
{
constexpr u32 WIDTH = 1280;
constexpr u32 HEIGHT = 720;
} // namespace

ViewportSize::ViewportSize(u32 width, u32 height) : width(width), height(height) {}

String ViewportSize::ToString() const
{
	Array<char, 40> buf;
	SPRINTF(buf.data(), buf.size(), "[%ux%u]", width, height);
	return String(buf.data());
}

ViewportData::ViewportData() : viewportSize{WIDTH, HEIGHT}, title("Untitled") {}
ViewportData::ViewportData(ViewportSize viewportSize, String title, ViewportStyle style)
	: viewportSize(std::move(viewportSize)), title(std::move(title)), style(style)
{
}
} // namespace LittleEngine
