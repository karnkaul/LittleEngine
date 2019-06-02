#include "stdafx.h"
#include "SFAssets.h"
#include "SFTypes.h"
#include "Core/CoreTypes.h"
#include "Core/Utils.h"
#include "Core/GData.h"
#include "Core/ArchiveReader.h"
#include "Core/Logger.h"

#pragma comment(lib, "freetype.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "gdi32.lib")
#if defined(_DEBUG)
#pragma comment(lib, "sfml-graphics-s-d.lib")
#pragma comment(lib, "sfml-window-s-d.lib")
#pragma comment(lib, "sfml-system-s-d.lib")
#else
#pragma comment(lib, "sfml-graphics-s.lib")
#pragma comment(lib, "sfml-window-s.lib")
#pragma comment(lib, "sfml-system-s.lib")
#endif

namespace LittleEngine
{
namespace
{
String GetFilesystemPath(const String& id, const String& pathPrefix)
{
	String prefix = pathPrefix.empty() ? "" : pathPrefix + "/";
	return prefix + id;
}
} // namespace

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
	if (!m_sfTexture.loadFromFile(GetFilesystemPath(m_id, pathPrefix)))
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

Vector2 TextureAsset::GetTextureSize() const
{
	return Cast(m_sfTexture.getSize());
}

FontAsset::FontAsset(String id, const String& pathPrefix) : Asset(std::move(id), AssetType::Font)
{
	if (!m_sfFont.loadFromFile(GetFilesystemPath(m_id, pathPrefix)))
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
	if (!m_sfSoundBuffer.loadFromFile(GetFilesystemPath(m_id, pathPrefix)))
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
	FileRW file(GetFilesystemPath(m_id, pathPrefix));
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
		m_text = Core::ArchiveReader::ToText(std::move(buffer));
		Strings::SubstituteChars(m_text, {{'\r', ' '}, {'\n', ' '}});
	}
}

const String& TextAsset::GetText() const
{
	return m_text;
}
} // namespace LittleEngine
