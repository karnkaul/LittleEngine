#include "stdafx.h"
#include "SFAssets.h"
#include "CoreTypes.h"
#include "Utils.h"

namespace LittleEngine
{
using GData = Core::GData;

namespace
{
AssetPaths Deserialise(GData& gData)
{
	return AssetPaths(gData.GetString("pathPrefix"), gData.GetS32("count"),
					  gData.GetString("assetPrefix"), gData.GetString("assetSuffix"));
}
} // namespace

void AssetManifest::AddDefinition(AssetDefinition&& definition)
{
	definitions.push_back(std::move(definition));
}

void AssetManifest::AddDefinition(const AssetType& type, AssetPaths&& resourcePaths)
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

AssetManifestData::AssetManifestData(const String& amfPath)
{
	Load(amfPath);
}

AssetManifest& AssetManifestData::GetManifest()
{
	return manifest;
}

void AssetManifestData::Load(const String& amfPath)
{
	FileRW reader(amfPath);
	GData data(reader.ReadAll(true));
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
			manifest.AddDefinition(AssetType::Texture, Deserialise(tSet));
		}
	}
	auto soundSets = data.GetVectorGData("soundSets");
	if (!soundSets.empty())
	{
		for (auto& sSet : soundSets)
		{
			manifest.AddDefinition(AssetType::Sound, Deserialise(sSet));
		}
	}
	manifest.archivePath = data.GetString("archivePath");
}

Asset::Asset(const String& path) : m_resourcePath(path)
{
}

Asset::~Asset()
{
	LOG_D("[%s] (Asset) destroyed", m_resourcePath.c_str());
}

const String& Asset::GetResourcePath() const
{
	return m_resourcePath;
}

TextureAsset::TextureAsset(const String& path) : Asset(path)
{
	if (!m_sfTexture.loadFromFile(m_resourcePath))
	{
		LOG_E("Could not load texture from [%s]!", m_resourcePath.c_str());
		throw AssetLoadException();
	}
}


 TextureAsset::TextureAsset(const String& path, const Vec<u8>& buffer) : Asset(path)
{
	 if (!m_sfTexture.loadFromMemory(buffer.data(), buffer.size()))
	 {
		 LOG_E("Could not load texture from [%s]!", m_resourcePath.c_str());
		 throw AssetLoadException();
	 }
 }

FontAsset::FontAsset(const String& path) : Asset(path)
{
	if (!m_sfFont.loadFromFile(m_resourcePath))
	{
		LOG_E("Could not load font from [%s]!", m_resourcePath.c_str());
		throw AssetLoadException();
	}
}

FontAsset::FontAsset(const String& path, const Vec<u8>& buffer) : Asset(path), m_fontBuffer(buffer)
{
	if (!m_sfFont.loadFromMemory(m_fontBuffer.data(), m_fontBuffer.size()))
	{
		LOG_E("Could not load font from [%s]!", m_resourcePath.c_str());
		throw AssetLoadException();
	}
}

SoundAsset::SoundAsset(const String& path, const Fixed& volumeScale)
	: Asset(path), m_volumeScale(Maths::Clamp01(volumeScale))
{
	if (!m_sfSoundBuffer.loadFromFile(path))
	{
		LOG_E("Could not load sound from [%s]!", m_resourcePath.c_str());
		throw AssetLoadException();
	}
}
SoundAsset::SoundAsset(const String& path, const Vec<u8>& buffer, const Fixed& volumeScale)
	: Asset(path), m_volumeScale(Maths::Clamp01(volumeScale))
{
	if (!m_sfSoundBuffer.loadFromMemory(buffer.data(), buffer.size()))
	{
		LOG_E("Could not load sound from [%s]!", m_resourcePath.c_str());
		throw AssetLoadException();
	}
}

MusicAsset::MusicAsset(const String& path, const Fixed& volumeScale)
	: Asset(path), m_volumeScale(Maths::Clamp01(volumeScale))
{
	m_bValid = m_sfMusic.openFromFile(path);
	if (!m_bValid)
	{
		LOG_E("Could not load music from [%s]!", m_resourcePath.c_str());
		throw AssetLoadException();
	}
}

Time MusicAsset::GetDuration() const
{
	return Time(m_sfMusic.getDuration().asMicroseconds());
}

AssetPaths::AssetPaths(const String& assetPath)
{
	assetPaths.push_back(assetPath);
}

AssetPaths::AssetPaths(InitList<String> assetPaths)
{
	for (const auto& path : assetPaths)
	{
		this->assetPaths.push_back(path);
	}
}

AssetPaths::AssetPaths(const String& pathPrefix, InitList<String> assetPaths)
{
	String prefix = pathPrefix.empty() ? "" : pathPrefix + "/";
	for (const auto& path : assetPaths)
	{
		this->assetPaths.emplace_back(prefix + path);
	}
}

AssetPaths::AssetPaths(const String& pathPrefix, u32 count, const String& assetPrefix, const String& assetSuffix)
{
	String prefix = pathPrefix.empty() ? "" : pathPrefix + "/";
	for (u32 i = 0; i < count; ++i)
	{
		String suffix = (i < 10) ? "0" + Strings::ToString(i) : Strings::ToString(i);
		suffix += assetSuffix;
		this->assetPaths.push_back(prefix + assetPrefix + suffix);
	}
}

String AssetPaths::GetRandom() const
{
	size_t index = Maths::Random::Range((size_t)0, assetPaths.size());
	return assetPaths[index];
}
} // namespace LittleEngine
