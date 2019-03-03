#include "stdafx.h"
#include "SFAssets.h"
#include "CoreTypes.h"
#include "Utils.h"
#include "ArchiveReader.h"

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
	auto music = data.GetVector("music");
	if (!music.empty())
	{
		for (const auto& mPath : music)
		{
			manifest.AddDefinition(AssetType::Music, mPath);
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
}

Asset::Asset(const String& id) : m_id(id)
{
}

Asset::~Asset()
{
	LOG_D("[%s] (Asset) destroyed", m_id.c_str());
}

const String& Asset::GetID() const
{
	return m_id;
}

TextureAsset::TextureAsset(const String& id, const String& pathPrefix) : Asset(id)
{
	if (!m_sfTexture.loadFromFile(GetFilesystemPath(id, pathPrefix)))
	{
		LOG_E("Could not load texture from file [%s]!", m_id.c_str());
		throw AssetLoadException();
	}
}

TextureAsset::TextureAsset(const String& id, const Vec<u8>& buffer) : Asset(id)
{
	 if (!m_sfTexture.loadFromMemory(buffer.data(), buffer.size()))
	 {
		 LOG_E("Could not load texture from buffer [%s]!", m_id.c_str());
		 throw AssetLoadException();
	 }
 }

FontAsset::FontAsset(const String& id, const String& pathPrefix) : Asset(id)
{
	if (!m_sfFont.loadFromFile(GetFilesystemPath(id, pathPrefix)))
	{
		LOG_E("Could not load font from file [%s]!", m_id.c_str());
		throw AssetLoadException();
	}
}

FontAsset::FontAsset(const String& id, const Vec<u8>& buffer) : Asset(id), m_fontBuffer(buffer)
{
	if (!m_sfFont.loadFromMemory(m_fontBuffer.data(), m_fontBuffer.size()))
	{
		LOG_E("Could not load font from buffer [%s]!", m_id.c_str());
		throw AssetLoadException();
	}
}

SoundAsset::SoundAsset(const String& id, const String& pathPrefix, const Fixed& volumeScale)
	: Asset(id), m_volumeScale(Maths::Clamp01(volumeScale))
{
	if (!m_sfSoundBuffer.loadFromFile(GetFilesystemPath(id, pathPrefix)))
	{
		LOG_E("Could not load sound from file [%s]!", m_id.c_str());
		throw AssetLoadException();
	}
}
SoundAsset::SoundAsset(const String& id, const Vec<u8>& buffer, const Fixed& volumeScale)
	: Asset(id), m_volumeScale(Maths::Clamp01(volumeScale))
{
	if (!m_sfSoundBuffer.loadFromMemory(buffer.data(), buffer.size()))
	{
		LOG_E("Could not load sound from buffer [%s]!", m_id.c_str());
		throw AssetLoadException();
	}
}

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

TextAsset::TextAsset(const String& id, const String& pathPrefix) : Asset(id)
{
	String prefix = pathPrefix.empty() ? "" : pathPrefix + "/";
	FileRW file(prefix + id);
	m_text = file.ReadAll(true);
}

TextAsset::TextAsset(const String& id, const Vec<u8>& buffer) : Asset(id)
{
	m_text = Core::ArchiveReader::ToText(buffer);
}

const String& TextAsset::GetText() const
{
	return m_text;
}
} // namespace LittleEngine
