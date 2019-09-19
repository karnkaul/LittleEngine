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

	String ToString() const;
};

struct ViewportData
{
	ViewportSize viewportSize;
	Vector2 viewSize;
	String title;
	ViewportStyle style = ViewportStyle::Default;

	ViewportData();
	ViewportData(ViewportSize viewportSize, Vector2 viewSize, String title, ViewportStyle style);
	ViewportData(ViewportData&&) = default;
	ViewportData& operator=(ViewportData&&) = default;
};

struct ViewportRecreateData
{
	std::optional<ViewportSize> oViewportSize;
	std::optional<ViewportStyle> oStyle;
	std::optional<String> oTitle;
};
} // namespace LittleEngine
