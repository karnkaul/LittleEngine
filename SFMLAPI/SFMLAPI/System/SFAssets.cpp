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

AssetIDContainer::AssetIDContainer(const String& assetPath)
{
	assetIDs.push_back(assetPath);
}

AssetIDContainer::AssetIDContainer(InitList<String> assetPaths)
{
	for (const auto& path : assetPaths)
	{
		this->assetIDs.push_back(path);
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
		this->assetIDs.push_back(prefix + assetPrefix + suffix);
	}
}

String AssetIDContainer::GetRandom() const
{
	size_t index = Maths::Random::Range((size_t)0, assetIDs.size());
	return assetIDs[index];
}

void AssetManifest::AddDefinition(AssetDefinition&& definition)
{
	definitions.push_back(std::move(definition));
}

void AssetManifest::AddDefinition(const AssetType& type, AssetIDContainer&& resourcePaths)
{
	definitions.emplace_back(type, std::move(resourcePaths));
}

void AssetManifest::Clear()
{
	definitions.clear();
}

void AssetManifest::ForEach(const std::function<void(const AssetDefinition& definition)>& Callback) const
{
	for (const auto& definition : definitions)
	{
		Callback(definition);
	}
}

AssetManifest& AssetManifestData::GetManifest()
{
	return manifest;
}

void AssetManifestData::Load(const String& amfPath)
{
	FileRW reader(amfPath);
	Deserialise(reader.ReadAll(true));
}

void AssetManifestData::Deserialise(const String& serialised)
{
	GData data(serialised);
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

Asset::Asset(const String& id, AssetType type) : m_id(id), m_type(type)
{
}

Asset::~Asset()
{
	if (!m_bError)
		LOG_I("%s [%s] destroyed", g_szAssetType[ToIdx(m_type)], m_id.c_str());
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

TextureAsset::TextureAsset(const String& id, const String& pathPrefix)
	: Asset(id, AssetType::Texture)
{
	if (!m_sfTexture.loadFromFile(GetFilesystemPath(id, pathPrefix)))
	{
		LOG_E("Could not load Texture from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
}

TextureAsset::TextureAsset(const String& id, const Vec<u8>& buffer) : Asset(id, AssetType::Texture)
{
	if (buffer.empty() || !m_sfTexture.loadFromMemory(buffer.data(), buffer.size()))
	{
		LOG_E("Could not load Texture from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
}

FontAsset::FontAsset(const String& id, const String& pathPrefix) : Asset(id, AssetType::Font)
{
	if (!m_sfFont.loadFromFile(GetFilesystemPath(id, pathPrefix)))
	{
		LOG_E("Could not load Font from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
}

FontAsset::FontAsset(const String& id, const Vec<u8>& buffer)
	: Asset(id, AssetType::Font), m_fontBuffer(buffer)
{
	if (buffer.empty() || !m_sfFont.loadFromMemory(m_fontBuffer.data(), m_fontBuffer.size()))
	{
		LOG_E("Could not load Font from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
}

SoundAsset::SoundAsset(const String& id, const String& pathPrefix, const Fixed& volumeScale)
	: Asset(id, AssetType::Sound), m_volumeScale(Maths::Clamp01(volumeScale))
{
	if (!m_sfSoundBuffer.loadFromFile(GetFilesystemPath(id, pathPrefix)))
	{
		LOG_E("Could not load Sound from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
}
SoundAsset::SoundAsset(const String& id, const Vec<u8>& buffer, const Fixed& volumeScale)
	: Asset(id, AssetType::Sound), m_volumeScale(Maths::Clamp01(volumeScale))
{
	if (buffer.empty() || !m_sfSoundBuffer.loadFromMemory(buffer.data(), buffer.size()))
	{
		LOG_E("Could not load Sound from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
}

TextAsset::TextAsset(const String& id, const String& pathPrefix) : Asset(id, AssetType::Text)
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

TextAsset::TextAsset(const String& id, const Vec<u8>& buffer) : Asset(id, AssetType::Text)
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
