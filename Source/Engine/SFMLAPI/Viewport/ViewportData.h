#pragma once
#include <optional>
#include "Core/CoreTypes.h"

namespace LittleEngine
{
enum class ViewportStyle
{
	Default = 0,
	Bordlerless
};

struct ViewportSize
{
	u32 width = 0;
	u32 height = 0;

	ViewportSize() = default;
	ViewportSize(u32 width, u32 height);

	std::string ToString() const;
};

struct ViewportData
{
	ViewportSize viewportSize;
	std::string title;
	ViewportStyle style = ViewportStyle::Default;

	ViewportData();
	ViewportData(ViewportSize viewportSize, std::string title, ViewportStyle style);
	ViewportData(ViewportData&&) = default;
	ViewportData& operator=(ViewportData&&) = default;
};

struct ViewportRecreateData
{
	std::optional<ViewportSize> oViewportSize;
	std::optional<ViewportStyle> oStyle;
	std::optional<std::string> oTitle;
};
} // namespace LittleEngine
