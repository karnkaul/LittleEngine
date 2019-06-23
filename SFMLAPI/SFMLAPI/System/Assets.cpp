#include "stdafx.h"
#include <filesystem>
#include "Assets.h"
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
std::pair<String, u64> GetFilesystemPath(const String& id, const String& pathPrefix)
{
	String path = (pathPrefix.empty() ? "" : pathPrefix + "/") + id;
	namespace fs = std::filesystem;
	std::error_code e;
	u64 size = static_cast<u64>(fs::file_size(path, e));
	if (e)
	{
		size = 0;
	}
	return std::make_pair(std::move(path), size);
}
} // namespace

Array<const char*, ToIdx(AssetType::_COUNT)> g_szAssetType = {"TextureAsset", "FontAsset",
															  "SoundAsset", "TextAsset"};

Asset::Asset(String id, AssetType type) : m_id(std::move(id)), m_type(type)
{
}

Asset::~Asset()
{
	if (!m_bError)
	{
		LOG_I("-- [%s] %s destroyed", m_id.c_str(), g_szAssetType[ToIdx(m_type)]);
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

u64 Asset::GetByteCount() const
{
	return m_byteCount;
}

TextureAsset::TextureAsset(String id, const String& pathPrefix)
	: Asset(std::move(id), AssetType::Texture)
{
	auto path = GetFilesystemPath(m_id, pathPrefix);
	if (!m_sfTexture.loadFromFile(path.first))
	{
		LOG_E("Could not load Texture from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
	m_byteCount = path.second;
}

TextureAsset::TextureAsset(String id, Vec<u8> buffer) : Asset(std::move(id), AssetType::Texture)
{
	m_byteCount = buffer.size();
	if (buffer.empty() || !m_sfTexture.loadFromMemory(buffer.data(), buffer.size()))
	{
		LOG_E("Could not load Texture from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
}

TextureAsset* TextureAsset::SetRepeated(bool bRepeat)
{
	m_sfTexture.setRepeated(bRepeat);
	return this;
}

Vector2 TextureAsset::GetTextureSize() const
{
	return Cast(m_sfTexture.getSize());
}

FontAsset::FontAsset(String id, const String& pathPrefix) : Asset(std::move(id), AssetType::Font)
{
	auto path = GetFilesystemPath(m_id, pathPrefix);
	if (!m_sfFont.loadFromFile(path.first))
	{
		LOG_E("Could not load Font from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
	m_byteCount = path.second;
}

FontAsset::FontAsset(String id, Vec<u8> buffer)
	: Asset(std::move(id), AssetType::Font), m_fontBuffer(std::move(buffer))
{
	m_byteCount = m_fontBuffer.size();
	if (m_fontBuffer.empty() || !m_sfFont.loadFromMemory(m_fontBuffer.data(), m_fontBuffer.size()))
	{
		LOG_E("Could not load Font from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
}

SoundAsset::SoundAsset(String id, const String& pathPrefix, Fixed volumeScale)
	: Asset(std::move(id), AssetType::Sound), m_volumeScale(Maths::Clamp01(volumeScale))
{
	auto path = GetFilesystemPath(m_id, pathPrefix);
	if (!m_sfSoundBuffer.loadFromFile(path.first))
	{
		LOG_E("Could not load Sound from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
	m_byteCount = path.second;
}
SoundAsset::SoundAsset(String id, Vec<u8> buffer, Fixed volumeScale)
	: Asset(std::move(id), AssetType::Sound), m_volumeScale(Maths::Clamp01(volumeScale))
{
	m_byteCount = buffer.size();
	if (buffer.empty() || !m_sfSoundBuffer.loadFromMemory(buffer.data(), buffer.size()))
	{
		LOG_E("Could not load Sound from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
}

TextAsset::TextAsset(String id, const String& pathPrefix) : Asset(std::move(id), AssetType::Text)
{
	auto path = GetFilesystemPath(m_id, pathPrefix);
	FileRW file(path.first);
	if (!file.Exists())
	{
		LOG_E("Could not load Text from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
	else
	{
		m_text = file.ReadAll(true);
	}
	m_byteCount = path.second;
}

TextAsset::TextAsset(String id, Vec<u8> buffer) : Asset(std::move(id), AssetType::Text)
{
	m_byteCount = buffer.size();
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
