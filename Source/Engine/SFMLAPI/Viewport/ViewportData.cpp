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
	String ret;
	ret.reserve(12);
	ret += "[";
	ret += std::to_string(width);
	ret += "x";
	ret += std::to_string(height);
	ret += "]";
	return ret;
}

ViewportData::ViewportData() : viewportSize{WIDTH, HEIGHT}, title("Untitled") {}
ViewportData::ViewportData(ViewportSize viewportSize, String title, ViewportStyle style)
	: viewportSize(std::move(viewportSize)), title(std::move(title)), style(style)
{
}
} // namespace LittleEngine
