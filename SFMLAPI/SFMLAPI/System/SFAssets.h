#pragma once
#include <string>
#include <unordered_map>
#include "SimpleTime.h"
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
	Texture = 0,
	Font = 1,
	Sound = 2,
	Text = 3,
};

extern const char* g_szAssetType[4];

// \brief Auto-constructs a set of generateable asset paths based on input parameters
struct AssetIDContainer
{
	Vec<String> assetIDs;

	AssetIDContainer(AssetIDContainer&&) = default;
	// Single asset's path
	AssetIDContainer(const String& assetPath);
	// Multiple assets' paths
	AssetIDContainer(InitList<String> assetPaths);
	// Multiple assets' paths with a prefix for each
	AssetIDContainer(const String& pathPrefix, InitList<String> assetPaths);
	// Multiple assets's paths with a prefix for each path, prefix and suffix for each asset
	AssetIDContainer(const String& pathPrefix, u32 count, const String& assetPrefix, const String& assetSuffix);

	// Get a random path from vector of paths
	String GetRandom() const;
};

// \brief Complete data specifying particular set of identical-type Assets
struct AssetDefinition
{
	AssetIDContainer assetIDs;
	AssetType type;

	AssetDefinition(AssetDefinition&&) = default;
	AssetDefinition(const AssetDefinition&) = delete;
	// Create an AssetDefinition given a resource type and its AssetPaths
	AssetDefinition(const AssetType& type, AssetIDContainer&& assetIDs)
		: assetIDs(std::move(assetIDs)), type(type)
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
	void AddDefinition(const AssetType& type, AssetIDContainer&& resourcePaths);
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
	// \brief Note: this is not a const function! Manifests are designed to move their data
	AssetManifest& GetManifest();
	// \brief Pass the path to the asset manifest file (.amf)
	void Load(const String& amfPath);
	void Deserialise(const String& serialised);
};

// \brief An Asset represents live data in memory that's ready to be used
class Asset
{
protected:
	String m_id;
	AssetType m_type;
	bool m_bError = false;

public:
	using Ptr = UPtr<Asset>;
	Asset() = delete;
	Asset(const String& id, AssetType type);
	virtual ~Asset();
	const char* GetID() const;
	bool IsError() const;
	AssetType GetType() const;

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
	TextureAsset(const String& id, const String& pathPrefix);
	TextureAsset(const String& id, const Vec<u8>& buffer);

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
	Vec<u8> m_fontBuffer;

public:
	FontAsset(const String& id, const String& pathPrefix);
	FontAsset(const String& id, const Vec<u8>& buffer);

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
	SoundAsset(const String& id, const String& pathPrefix, const Fixed& volumeScale = Fixed::One);
	SoundAsset(const String& id, const Vec<u8>& buffer, const Fixed& volumeScale = Fixed::One);

private:
	friend class SoundPlayer;
};

class TextAsset : public Asset
{
private:
	String m_text;

public:
	TextAsset(const String& id, const String& pathPrefix);
	TextAsset(const String& id, const Vec<u8>& buffer);

	const String& GetText() const;
};
} // namespace LittleEngine
