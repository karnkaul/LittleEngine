#include "stdafx.h"
#include "SFAssets.h"
#include "CoreTypes.h"
#include "Utils.h"
#include "GData.h"
#include "ArchiveReader.h"
#include "Logger.h"

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

String GetFilesystemPath(const String& id, const String& pathPrefix)
{
	String prefix = pathPrefix.empty() ? "" : pathPrefix + "/";
	return prefix + id;
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
	size_t index = Maths::Random::Range(static_cast<size_t>(0), assetIDs.size());
	return assetIDs[index];
}

AssetDefinition::AssetDefinition(AssetType type, AssetIDContainer assetIDs)
	: assetIDs(std::move(assetIDs)), type(type)
{
}

AssetManifest::AssetManifest(Vec<AssetDefinition> assetDefinitions)
	: definitions(std::move(assetDefinitions))
{
}

void AssetManifest::AddDefinition(AssetDefinition definition)
{
	definitions.emplace_back(std::move(definition));
}

void AssetManifest::AddDefinition(AssetType type, AssetIDContainer resourcePaths)
{
	definitions.emplace_back(type, std::move(resourcePaths));
}

void AssetManifest::Clear()
{
	definitions.clear();
}

void AssetManifest::ForEach(const std::function<void(const AssetDefinition& definition)>& callback) const
{
	for (const auto& definition : definitions)
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

Asset::Asset(String id, AssetType type) : m_id(std::move(id)), m_type(type)
{
}

Asset::~Asset()
{
	if (!m_bError)
	{
		LOG_I("%s [%s] destroyed", g_szAssetType[ToIdx(m_type)], m_id.c_str());
	}
}

const char* Asset::GetID() const
{
	return m_id.c_str();
}

bool Asset::IsError() const
{
	return m_bError;
}

AssetType Asset::GetType() const
{
	return m_type;
}

TextureAsset::TextureAsset(String id, const String& pathPrefix)
	: Asset(std::move(id), AssetType::Texture)
{
	if (!m_sfTexture.loadFromFile(GetFilesystemPath(id, pathPrefix)))
	{
		LOG_E("Could not load Texture from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
}

TextureAsset::TextureAsset(String id, Vec<u8> buffer) : Asset(std::move(id), AssetType::Texture)
{
	if (buffer.empty() || !m_sfTexture.loadFromMemory(buffer.data(), buffer.size()))
	{
		LOG_E("Could not load Texture from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
}

FontAsset::FontAsset(String id, const String& pathPrefix) : Asset(std::move(id), AssetType::Font)
{
	if (!m_sfFont.loadFromFile(GetFilesystemPath(id, pathPrefix)))
	{
		LOG_E("Could not load Font from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
}

FontAsset::FontAsset(String id, Vec<u8> buffer)
	: Asset(std::move(id), AssetType::Font), m_fontBuffer(std::move(buffer))
{
	if (m_fontBuffer.empty() || !m_sfFont.loadFromMemory(m_fontBuffer.data(), m_fontBuffer.size()))
	{
		LOG_E("Could not load Font from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
}

SoundAsset::SoundAsset(String id, const String& pathPrefix, Fixed volumeScale)
	: Asset(std::move(id), AssetType::Sound), m_volumeScale(Maths::Clamp01(volumeScale))
{
	if (!m_sfSoundBuffer.loadFromFile(GetFilesystemPath(id, pathPrefix)))
	{
		LOG_E("Could not load Sound from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
}
SoundAsset::SoundAsset(String id, Vec<u8> buffer, Fixed volumeScale)
	: Asset(std::move(id), AssetType::Sound), m_volumeScale(Maths::Clamp01(volumeScale))
{
	if (buffer.empty() || !m_sfSoundBuffer.loadFromMemory(buffer.data(), buffer.size()))
	{
		LOG_E("Could not load Sound from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
}

TextAsset::TextAsset(String id, const String& pathPrefix) : Asset(std::move(id), AssetType::Text)
{
	String prefix = pathPrefix.empty() ? "" : pathPrefix + "/";
	FileRW file(prefix + id);
	if (!file.Exists())
	{
		LOG_E("Could not load Text from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
	else
	{
		m_text = file.ReadAll(true);
	}
}

TextAsset::TextAsset(String id, Vec<u8> buffer) : Asset(std::move(id), AssetType::Text)
{
	if (buffer.empty())
	{
		LOG_E("Could not load Text from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
	else
	{
		m_text = Core::ArchiveReader::ToText(buffer);
	}
}

const String& TextAsset::GetText() const
{
	return m_text;
}
} // namespace LittleEngine
