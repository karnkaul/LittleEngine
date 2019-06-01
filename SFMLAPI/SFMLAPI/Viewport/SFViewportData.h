#pragma once
#include <optional>
#include "Core/CoreTypes.h"

namespace LittleEngine
{
enum class SFViewportStyle
{
	Default,
	Bordlerless
};

struct SFViewportSize
{
	u32 width = 0;
	u32 height = 0;
};

struct SFViewportData
{
	SFViewportSize viewportSize;
	Vector2 viewSize;
	String title;
	SFViewportStyle style = SFViewportStyle::Default;

	SFViewportData();
	SFViewportData(SFViewportSize viewportSize, Vector2 viewSize, String title, SFViewportStyle style);
	SFViewportData(SFViewportData&&) = default;
	SFViewportData& operator=(SFViewportData&&) = default;
};

struct SFViewportRecreateData
{
	std::optional<SFViewportSize> oViewportSize;
	std::optional<SFViewportStyle> oSstyle;
	std::optional<String> oTitle;

	SFViewportRecreateData(SFViewportSize size);
	SFViewportRecreateData(SFViewportStyle style);
	SFViewportRecreateData(String title);
	SFViewportRecreateData(SFViewportRecreateData&&) = default;
	SFViewportRecreateData& operator=(SFViewportRecreateData&&) = default;
};
} // namespace LittleEngine
