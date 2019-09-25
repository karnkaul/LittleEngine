#pragma once

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

extern Array<VString, ToIdx(AssetType::_COUNT)> g_szAssetType;
} // namespace LittleEngine
