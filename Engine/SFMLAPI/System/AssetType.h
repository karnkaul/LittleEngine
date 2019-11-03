#pragma once
#include <array>
#include <string_view>

namespace LittleEngine
{
enum class AssetType : u8
{
	Texture = 0,
	Font,
	Sound,
	Text,
	File,
	_COUNT
};

extern std::array<std::string_view, ToIdx(AssetType::_COUNT)> g_szAssetType;
} // namespace LittleEngine
