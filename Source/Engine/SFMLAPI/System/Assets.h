#pragma once
#include "Core/CoreTypes.h"
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "AssetType.h"

namespace LittleEngine
{
// \brief An Asset represents live data in memory that's ready to be used
class Asset : private NoCopy
{
public:
	static std::string s_pathPrefix;

protected:
	std::vector<u8> m_buffer;
	std::string m_id;
	Pair<std::string, u64> m_pathSize;
	AssetType m_type;
	u64 m_byteCount = 0;
	bool m_bError = true;

public:
	static bool DoesFileExist(std::string_view id);
	static Pair<std::string, u64> FilePathAndSize(std::string_view id);

public:
	Asset() = delete;
	virtual ~Asset();

protected:
	Asset(std::string id, AssetType type = AssetType::File);
	Asset(std::string id, std::vector<u8> buffer, AssetType type = AssetType::File);

public:
	std::string_view ID() const;
	bool IsError() const;
	AssetType Type() const;
	u64 ByteCount() const;
	bool WriteBytes(const std::string& path);
};

// \brief TextureAsset is an image texture copied to VRAM
class TextureAsset : public Asset
{
public:
	sf::Texture m_sfTexture;

public:
	TextureAsset(std::string id);
	TextureAsset(std::string id, std::vector<u8> buffer);

	TextureAsset* SetRepeated(bool bRepeat);

	Vector2 TextureSize() const;

private:
	friend class SFQuad;
	friend class SFQuadVec;
	friend class SFPrimitive;
};

// \brief FontAsset is a font type loaded in RAM
class FontAsset : public Asset
{
public:
	sf::Font m_sfFont;

public:
	FontAsset(std::string id);
	FontAsset(std::string id, std::vector<u8> buffer);

private:
	friend class SFText;
	friend class SFPrimitive;
};

// \brief SoundAsset is an audio asset fully loaded in RAM
class SoundAsset : public Asset
{
public:
	sf::SoundBuffer m_sfSoundBuffer;
	Fixed m_volumeScale = Fixed::One;

public:
	SoundAsset(std::string id, Fixed volumeScale = Fixed::One);
	SoundAsset(std::string id, std::vector<u8> buffer, Fixed volumeScale = Fixed::One);

private:
	friend class SoundPlayer;
};

class TextAsset : public Asset
{
private:
	std::string m_text;

public:
	TextAsset(std::string id);
	TextAsset(std::string id, std::vector<u8> buffer);

	const std::string& Text() const;
};
} // namespace LittleEngine
