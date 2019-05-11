#pragma once
#include <optional>
#include "Core/CoreTypes.h"

namespace LittleEngine
{
enum class SFWindowStyle
{
	Default,
	Bordlerless
};

struct SFWindowSize
{
	u32 width = 0;
	u32 height = 0;

	SFWindowSize();
	SFWindowSize(u32 width, u32 height);
	SFWindowSize(SFWindowSize&&) = default;
	SFWindowSize& operator=(SFWindowSize&&) = default;
};

struct SFWindowData
{
	SFWindowSize windowSize;
	Vector2 viewSize;
	String title;
	SFWindowStyle style = SFWindowStyle::Default;

	SFWindowData();
	SFWindowData(SFWindowSize size, Vector2 viewSize, String title, SFWindowStyle style);
	SFWindowData(SFWindowData&&) = default;
	SFWindowData& operator=(SFWindowData&&) = default;
};

struct SFWindowRecreateData
{
	std::optional<SFWindowSize> oWindowSize;
	std::optional<SFWindowStyle> oSstyle;
	std::optional<String> oTitle;

	SFWindowRecreateData(SFWindowSize size);
	SFWindowRecreateData(SFWindowStyle style);
	SFWindowRecreateData(String title);
	SFWindowRecreateData(SFWindowRecreateData&&) = default;
	SFWindowRecreateData& operator=(SFWindowRecreateData&&) = default;
};
} // namespace LittleEngine
