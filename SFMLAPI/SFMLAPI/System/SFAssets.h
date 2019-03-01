#pragma once
#include <string>
#include <unordered_map>
#include "SFTime.h"
#include "CoreTypes.h"
#include "Logger.h"
#include "Utils.h"
#include "GData.h"
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

namespace LittleEngine
{
enum class AssetType
{
	Texture,
	Font,
	Sound,
	Music,
};

// \brief Auto-constructs a set of generateable asset paths based on input parameters
struct AssetPaths
{
	Vec<String> assetPaths;

	AssetPaths(AssetPaths&&) = default;
	// Single asset's path
	AssetPaths(const String& assetPath);
	// Multiple assets' paths
	AssetPaths(InitList<String> assetPaths);
	// Multiple assets' paths with a prefix for each
	AssetPaths(const String& pathPrefix, InitList<String> assetPaths);
	// Multiple assets's paths with a prefix for each path, prefix and suffix for each asset
	AssetPaths(const String& pathPrefix, u32 count, const String& assetPrefix, const String& assetSuffix);

	// Get a random path from vector of paths
	String GetRandom() const;
};

// \brief Complete data specifying particular set of identical-type Assets
struct AssetDefinition
{
	AssetPaths resourcePaths;
	AssetType type;

	AssetDefinition(AssetDefinition&&) = default;
	AssetDefinition(const AssetDefinition&) = delete;
	// Create an AssetDefinition given a resource type and its AssetPaths
	AssetDefinition(const AssetType& type, AssetPaths&& resourcePaths)
		: resourcePaths(std::move(resourcePaths)), type(type)
	{
	}
};

// \brief Collection of AssetDefinitions
struct AssetManifest
{
public:
	Vec<AssetDefinition> definitions;

public:
	AssetManifest() = default;
	AssetManifest(Vec<AssetDefinition>&& assetDefinitions)
		: definitions(std::move(assetDefinitions))
	{
	}

	// Add definition to the manifest
	void AddDefinition(AssetDefinition&& definition);
	// Create and add an AssetDefinition to the manifest
	void AddDefinition(const AssetType& type, AssetPaths&& resourcePaths);
	void Clear();
	// Convenience callback that iterates through each AssetDefinition
	void ForEach(const std::function<void(const AssetDefinition& definition)>& Callback) const;
};

// \brief Generates AssetManifest from serialised data loaded from a file, given its path
struct AssetManifestData
{
private:
	AssetManifest manifest;

public:
	AssetManifestData() = default;
	AssetManifestData(AssetManifestData&&) = default;
	AssetManifestData& operator=(AssetManifestData&&) = default;
	// \brief Pass the path to the asset manifest file (.amf)
	AssetManifestData(const String& amfPath);

	// \brief Note: this is not a const function! Manifests are designed to move their data
	AssetManifest& GetManifest();
	// \brief Pass the path to the asset manifest file (.amf)
	void Load(const String& amfPath);
};

// \brief An Asset represents live data in memory that's ready to be used
class Asset
{
protected:
	String m_resourcePath;

public:
	using Ptr = UPtr<Asset>;
	Asset() = delete;
	// Path must be relative to the root Asset directory
	Asset(const String& path);
	virtual ~Asset();
	const String& GetResourcePath() const;

private:
	Asset(const Asset&) = delete;
	Asset& operator=(const Asset&) = delete;
};

// \brief TextureAsset is an image texture copied to VRAM
class TextureAsset : public Asset
{
private:
	sf::Texture m_sfTexture;

public:
	// Path must be relative to the root Asset directory
	TextureAsset(const String& path);

private:
	// Prevents having to expose texture to code outside SFMLInterface
	friend class SFSprite;
	friend class SFPrimitive;
};

// \brief FontAsset is a font type loaded in RAM
class FontAsset : public Asset
{
private:
	sf::Font m_sfFont;

public:
	// Path must be relative to the root Asset directory
	FontAsset(const String& path);

private:
	friend class SFText;
	friend class SFPrimitive;
};

// \brief SoundAsset is an audio asset fully loaded in RAM
class SoundAsset : public Asset
{
private:
	sf::SoundBuffer m_sfSoundBuffer;
	Fixed m_volumeScale = Fixed::One;

public:
	// Path must be relative to the root Asset directory
	SoundAsset(const String& path, const Fixed& volumeScale = Fixed::One);

private:
	friend class SoundPlayer;
};

// \brief MusicAsset points to a streamed music file on storage
class MusicAsset : public Asset
{
public:
	bool m_bValid;

private:
	sf::Music m_sfMusic;
	Fixed m_volumeScale = Fixed::One;

public:
	// Path must be relative to the root Asset directory
	MusicAsset(const String& path, const Fixed& volumeScale = Fixed::One);

public:
	Time GetDuration() const;

private:
	friend class MusicPlayer;
};
} // namespace LittleEngine
