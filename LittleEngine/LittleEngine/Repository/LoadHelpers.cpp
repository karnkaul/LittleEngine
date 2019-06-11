#include "stdafx.h"
#include "Core/GData.h"
#include "Core/Utils.h"
#include "LoadHelpers.h"

namespace LittleEngine
{
using GData = Core::GData;

namespace
{
AssetIDContainer DeserialiseAssetPaths(GData& gData)
{
	return AssetIDContainer(gData.GetString("pathPrefix"), gData.GetS32("count"),
							gData.GetString("assetPrefix"), gData.GetString("assetSuffix"));
}
} // namespace

const char* g_szAssetType[4] = {"TextureAsset", "FontAsset", "SoundAsset", "TextAsset"};

AssetIDContainer::AssetIDContainer(String assetPath)
{
	assetIDs.emplace_back(std::move(assetPath));
}

AssetIDContainer::AssetIDContainer(InitList<String> assetPaths)
{
	for (const auto& path : assetPaths)
	{
		this->assetIDs.emplace_back(path);
	}
}

AssetIDContainer::AssetIDContainer(const String& pathPrefix, InitList<String> assetPaths)
{
	String prefix = pathPrefix.empty() ? "" : pathPrefix + "/";
	for (const auto& path : assetPaths)
	{
		this->assetIDs.emplace_back(prefix + path);
	}
}

AssetIDContainer::AssetIDContainer(const String& pathPrefix, u32 count, const String& assetPrefix, const String& assetSuffix)
{
	String prefix = pathPrefix.empty() ? "" : pathPrefix + "/";
	for (u32 i = 0; i < count; ++i)
	{
		String suffix = (i < 10) ? "0" + Strings::ToString(i) : Strings::ToString(i);
		suffix += assetSuffix;
		this->assetIDs.emplace_back(prefix + assetPrefix + suffix);
	}
}

String AssetIDContainer::GetRandom() const
{
	size_t index = Maths::Random::Range(ToIdx(0), assetIDs.size());
	return assetIDs[index];
}

AssetDefinition::AssetDefinition(AssetType type, AssetIDContainer assetIDs)
	: assetIDs(std::move(assetIDs)), type(type)
{
}

AssetManifest::AssetManifest(Vec<AssetDefinition> assetDefinitions)
	: assetDefs(std::move(assetDefinitions))
{
}

void AssetManifest::AddDefinition(AssetDefinition definition)
{
	assetDefs.emplace_back(std::move(definition));
}

void AssetManifest::AddDefinition(AssetType type, AssetIDContainer resourcePaths)
{
	assetDefs.emplace_back(type, std::move(resourcePaths));
}

void AssetManifest::Clear()
{
	assetDefs.clear();
}

void AssetManifest::ForEach(std::function<void(const AssetDefinition& definition)> callback) const
{
	for (const auto& definition : assetDefs)
	{
		callback(definition);
	}
}

AssetManifest& AssetManifestData::GetManifest()
{
	return manifest;
}

void AssetManifestData::Load(String amfPath)
{
	FileRW reader(std::move(amfPath));
	Deserialise(reader.ReadAll(true));
}

void AssetManifestData::Deserialise(String serialised)
{
	GData data(std::move(serialised));
	manifest.Clear();
	auto textures = data.GetVector("textures");
	if (!textures.empty())
	{
		for (const auto& tPath : textures)
		{
			manifest.AddDefinition(AssetType::Texture, tPath);
		}
	}
	auto sounds = data.GetVector("sounds");
	if (!sounds.empty())
	{
		for (const auto& sPath : sounds)
		{
			manifest.AddDefinition(AssetType::Sound, sPath);
		}
	}
	auto fonts = data.GetVector("fonts");
	if (!fonts.empty())
	{
		for (const auto& fPath : fonts)
		{
			manifest.AddDefinition(AssetType::Font, fPath);
		}
	}
	auto texts = data.GetVector("texts");
	if (!texts.empty())
	{
		for (const auto& txtPath : texts)
		{
			manifest.AddDefinition(AssetType::Text, txtPath);
		}
	}
	auto textureSets = data.GetVectorGData("textureSets");
	if (!textureSets.empty())
	{
		for (auto& tSet : textureSets)
		{
			manifest.AddDefinition(AssetType::Texture, DeserialiseAssetPaths(tSet));
		}
	}
	auto soundSets = data.GetVectorGData("soundSets");
	if (!soundSets.empty())
	{
		for (auto& sSet : soundSets)
		{
			manifest.AddDefinition(AssetType::Sound, DeserialiseAssetPaths(sSet));
		}
	}
	auto textSets = data.GetVectorGData("textSets");
	if (!textSets.empty())
	{
		for (auto& txtSet : textSets)
		{
			manifest.AddDefinition(AssetType::Text, DeserialiseAssetPaths(txtSet));
		}
	}
}
} // namespace LittleEngine