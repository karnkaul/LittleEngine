#include "Core/Game/GData.h"
#include "AssetManifest.h"

namespace LittleEngine
{
using GData = Core::GData;

namespace
{
AssetIDContainer DeserialiseAssetPaths(GData& gData)
{
	return AssetIDContainer(gData.GetString("pathPrefix"), static_cast<u32>(gData.GetS32("count")), gData.GetString("assetPrefix"),
							gData.GetString("assetSuffix"));
}
} // namespace

AssetIDContainer::AssetIDContainer(std::string assetPath)
{
	assetIDs.emplace_back(std::move(assetPath));
}

AssetIDContainer::AssetIDContainer(std::initializer_list<std::string> assetPaths)
{
	for (const auto& path : assetPaths)
	{
		this->assetIDs.emplace_back(path);
	}
}

AssetIDContainer::AssetIDContainer(const std::string& pathPrefix, std::initializer_list<std::string> assetPaths)
{
	std::string prefix = pathPrefix.empty() ? "" : pathPrefix + "/";
	for (const auto& path : assetPaths)
	{
		this->assetIDs.emplace_back(prefix + path);
	}
}

AssetIDContainer::AssetIDContainer(const std::string& pathPrefix, u32 count, const std::string& assetPrefix, const std::string& assetSuffix)
{
	std::string prefix = pathPrefix.empty() ? "" : pathPrefix + "/";
	for (u32 i = 0; i < count; ++i)
	{
		std::string suffix = (i < 10) ? "0" + Strings::ToString(i) : Strings::ToString(i);
		suffix += assetSuffix;
		this->assetIDs.emplace_back(prefix + assetPrefix + suffix);
	}
}

std::string AssetIDContainer::Random() const
{
	size_t index = Maths::Random::Range(ToIdx(0), assetIDs.size());
	return assetIDs[index];
}

AssetDefinition::AssetDefinition(AssetType type, AssetIDContainer assetIDs) : assetIDs(std::move(assetIDs)), type(type) {}

AssetManifest::AssetManifest(std::vector<AssetDefinition> assetDefinitions) : assetDefs(std::move(assetDefinitions)) {}

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

AssetManifest& AssetManifestData::Manifest()
{
	return manifest;
}

void AssetManifestData::Load(std::string amfPath)
{
	FileRW reader(std::move(amfPath));
	Deserialise(reader.Load(true));
}

void AssetManifestData::Deserialise(std::string serialised)
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
