#pragma once
#include "Core/CoreTypes.h"
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "AssetType.h"

namespace LittleEngine
{
// \brief An Asset represents live data in memory that's ready to be used
class Asset
{
public:
	static String s_pathPrefix;

protected:
	Vec<u8> m_buffer;
	String m_id;
	Pair<String, u64> m_pathSize;
	AssetType m_type;
	u64 m_byteCount = 0;
	bool m_bError = true;

public:
	static bool DoesFileExist(VString id);
	static Pair<String, u64> FilePathAndSize(VString id);

public:
	Asset() = delete;
	virtual ~Asset();

protected:
	Asset(String id, AssetType type = AssetType::File);
	Asset(String id, Vec<u8> buffer, AssetType type = AssetType::File);

public:
	VString ID() const;
	bool IsError() const;
	AssetType Type() const;
	u64 ByteCount() const;
	bool WriteBytes(const String& path);

public:
	Asset(const Asset&) = delete;
	Asset& operator=(const Asset&) = delete;
};

// \brief TextureAsset is an image texture copied to VRAM
class TextureAsset : public Asset
{
public:
	sf::Texture m_sfTexture;

public:
	TextureAsset(String id);
	TextureAsset(String id, Vec<u8> buffer);

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
	FontAsset(String id);
	FontAsset(String id, Vec<u8> buffer);

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
	SoundAsset(String id, Fixed volumeScale = Fixed::One);
	SoundAsset(String id, Vec<u8> buffer, Fixed volumeScale = Fixed::One);

private:
	friend class SoundPlayer;
};

class TextAsset : public Asset
{
private:
	String m_text;

public:
	TextAsset(String id);
	TextAsset(String id, Vec<u8> buffer);

	const String& Text() const;
};
} // namespace LittleEngine
