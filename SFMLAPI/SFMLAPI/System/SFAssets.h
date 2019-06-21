#pragma once
#include "Core/CoreTypes.h"
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "SFAssetType.h"

namespace LittleEngine
{
// \brief An Asset represents live data in memory that's ready to be used
class Asset
{
protected:
	String m_id;
	AssetType m_type;
	u64 m_byteCount = 0;
	bool m_bError = false;

public:
	using Ptr = UPtr<Asset>;
	Asset() = delete;
	Asset(String id, AssetType type);
	virtual ~Asset();

	const char* GetID() const;
	bool IsError() const;
	AssetType GetType() const;
	u64 GetByteCount() const;

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
	TextureAsset(String id, const String& pathPrefix);
	TextureAsset(String id, Vec<u8> buffer);

	TextureAsset* SetRepeated(bool bRepeat);

	Vector2 GetTextureSize() const;

private:
	friend class SFQuad;
	friend class SFQuadVec;
	friend class SFPrimitive;
};

// \brief FontAsset is a font type loaded in RAM
class FontAsset : public Asset
{
private:
	Vec<u8> m_fontBuffer;

public:
	sf::Font m_sfFont;

public:
	FontAsset(String id, const String& pathPrefix);
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
	SoundAsset(String id, const String& pathPrefix, Fixed volumeScale = Fixed::One);
	SoundAsset(String id, Vec<u8> buffer, Fixed volumeScale = Fixed::One);

private:
	friend class SoundPlayer;
};

class TextAsset : public Asset
{
private:
	String m_text;

public:
	TextAsset(String id, const String& pathPrefix);
	TextAsset(String id, Vec<u8> buffer);

	const String& GetText() const;
};
} // namespace LittleEngine
