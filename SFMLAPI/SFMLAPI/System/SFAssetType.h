#pragma once

namespace LittleEngine
{
enum class AssetType
{
	Texture = 0,
	Font,
	Sound,
	Text,
	_COUNT
};

extern Array<const char*, ToIdx(AssetType::_COUNT)> g_szAssetType;
} // namespace LittleEngine
