#include <fstream>
#include "Core/OS.h"
#if ENABLED(STD_FILESYSTEM)
#include <filesystem>
#endif
#include "Assets.h"
#include "SFTypes.h"
#include "Core/CoreTypes.h"
#include "Core/GData.h"
#include "Core/Logger.h"
#include "Core/OS.h"
#include "Core/Utils.h"

namespace LittleEngine
{
Array<VString, ToIdx(AssetType::_COUNT)> g_szAssetType = {"TextureAsset", "FontAsset", "SoundAsset", "TextAsset", "FileAsset"};

String Asset::s_pathPrefix;

bool Asset::DoesFileExist(VString id)
{
	String fullPath = s_pathPrefix;
	fullPath += id;
	return OS::DoesFileExist(OS::Env()->FullPath(fullPath));
}

Pair<String, u64> Asset::FilePathAndSize(VString id)
{
	String fullPath = s_pathPrefix;
	fullPath += id;
	fullPath = OS::Env()->FullPath(fullPath.c_str());
	u64 size = OS::FileSize(fullPath.c_str());
	return std::make_pair(std::move(fullPath), size);
}

Asset::Asset(String id, AssetType type) : m_id(std::move(id)), m_type(type)
{
	m_pathSize = FilePathAndSize(m_id);
#if ENABLED(FILESYSTEM_ASSETS)
	if (m_type == AssetType::File)
	{
		m_byteCount = m_pathSize.second;
		std::ifstream inFile(m_pathSize.first.c_str(), std::ios::binary);
		inFile.unsetf(std::ios::skipws);
		if (inFile.good())
		{
			static const u64 MAX_SIZE = 8192;
			m_buffer.reserve(MAX_SIZE);
			m_buffer.insert(m_buffer.begin(), std::istream_iterator<char>(inFile), std::istream_iterator<char>());
			m_buffer.push_back('\0');
			m_bError = m_buffer.empty();
		}
	}
#endif
}

Asset::Asset(String id, Vec<u8> buffer, AssetType type) : m_buffer(std::move(buffer)), m_id(std::move(id)), m_type(type)
{
	m_bError = !m_buffer.empty();
	m_byteCount = m_buffer.size();
}

Asset::~Asset()
{
	if (!m_bError)
	{
		LOG_I("-- [%s] %s destroyed", m_id.c_str(), g_szAssetType[ToIdx(m_type)].data());
	}
}

VString Asset::ID() const
{
	return m_id;
}

bool Asset::IsError() const
{
	return m_bError;
}

AssetType Asset::Type() const
{
	return m_type;
}

u64 Asset::ByteCount() const
{
	return m_byteCount;
}

bool Asset::WriteBytes(const String& path)
{
	bool bSuccess = false;
	if (!m_buffer.empty())
	{
		std::ofstream outFile(path.c_str(), std::ios::binary | std::ios::out);
		if (outFile.is_open())
		{
			outFile.write(reinterpret_cast<char*>(m_buffer.data()), static_cast<std::streamsize>(m_buffer.size()));
			outFile.close();
			bSuccess = std::ifstream(path.c_str()).good();
		}
	}
#if ENABLED(FILESYSTEM_ASSETS)
	if (!bSuccess)
	{
		auto sourcePath = FilePathAndSize(m_id);
		if (std::ifstream(sourcePath.first.c_str()).good())
		{
#if ENABLED(STD_FILESYSTEM)
			std::filesystem::copy(sourcePath.first.c_str(), path.c_str());
#else
			std::ifstream source(sourcePath.first.c_str(), std::ios::binary);
			std::ofstream dest(path.c_str(), std::ios::binary);
			source.seekg(0, std::ios::end);
			auto size = source.tellg();
			source.seekg(0);
			Vec<char> buffer(size, '\0');
			source.read(buffer.data(), size);
			dest.write(buffer.data(), size);
#endif
			bSuccess = std::ifstream(path.c_str()).good();
		}
	}
#endif
#if ENABLED(DEBUG_LOGGING)
	if (bSuccess)
	{
		auto size = Core::FriendlySize(m_byteCount);
		LOG_D("[%s] %s wrote [%.2f%s] to [%s]", m_id.c_str(), g_szAssetType[ToIdx(m_type)].data(), size.first, size.second,
			  m_pathSize.first.c_str());
	}
#endif
	return bSuccess;
}

TextureAsset::TextureAsset(String id) : Asset(std::move(id), AssetType::Texture)
{
	m_byteCount = m_pathSize.second;
	if (!m_sfTexture.loadFromFile(m_pathSize.first))
	{
		LOG_E("Could not load Texture from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
	else
	{
		m_bError = false;
	}
}

TextureAsset::TextureAsset(String id, Vec<u8> buffer) : Asset(std::move(id), std::move(buffer), AssetType::Texture)
{
	m_byteCount = m_buffer.size();
	if (m_buffer.empty() || !m_sfTexture.loadFromMemory(m_buffer.data(), m_buffer.size()))
	{
		LOG_E("Could not load Texture from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
	else
	{
		m_bError = false;
	}
}

TextureAsset* TextureAsset::SetRepeated(bool bRepeat)
{
	m_sfTexture.setRepeated(bRepeat);
	return this;
}

Vector2 TextureAsset::TextureSize() const
{
	return Cast(m_sfTexture.getSize());
}

FontAsset::FontAsset(String id) : Asset(std::move(id), AssetType::Font)
{
	if (!m_sfFont.loadFromFile(m_pathSize.first))
	{
		LOG_E("Could not load Font from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
	else
	{
		m_bError = false;
	}
	m_byteCount = m_pathSize.second;
}

FontAsset::FontAsset(String id, Vec<u8> buffer) : Asset(std::move(id), std::move(buffer), AssetType::Font)
{
	m_byteCount = m_buffer.size();
	if (m_buffer.empty() || !m_sfFont.loadFromMemory(m_buffer.data(), m_buffer.size()))
	{
		LOG_E("Could not load Font from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
	else
	{
		m_bError = false;
	}
}

SoundAsset::SoundAsset(String id, Fixed volumeScale) : Asset(std::move(id), AssetType::Sound), m_volumeScale(Maths::Clamp01(volumeScale))
{
	if (!m_sfSoundBuffer.loadFromFile(m_pathSize.first))
	{
		LOG_E("Could not load Sound from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
	else
	{
		m_bError = false;
	}
	m_byteCount = m_pathSize.second;
}
SoundAsset::SoundAsset(String id, Vec<u8> buffer, Fixed volumeScale)
	: Asset(std::move(id), std::move(buffer), AssetType::Sound), m_volumeScale(Maths::Clamp01(volumeScale))
{
	m_byteCount = m_buffer.size();
	if (m_buffer.empty() || !m_sfSoundBuffer.loadFromMemory(m_buffer.data(), m_buffer.size()))
	{
		LOG_E("Could not load Sound from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
	else
	{
		m_bError = false;
	}
}

TextAsset::TextAsset(String id) : Asset(std::move(id), AssetType::Text)
{
	FileRW file(m_pathSize.first);
	if (!file.Exists())
	{
		LOG_E("Could not load Text from filesystem [%s]!", m_id.c_str());
		m_bError = true;
	}
	else
	{
		m_text = file.Load(false);
		m_bError = false;
	}
	m_byteCount = m_pathSize.second;
}

TextAsset::TextAsset(String id, Vec<u8> buffer) : Asset(std::move(id), std::move(buffer), AssetType::Text)
{
	m_byteCount = m_buffer.size();
	if (m_buffer.empty())
	{
		LOG_E("Could not load Text from buffer [%s]!", m_id.c_str());
		m_bError = true;
	}
	else
	{
		m_bError = false;
		m_text = Strings::ToText(m_buffer);
	}
}

const String& TextAsset::Text() const
{
	return m_text;
}
} // namespace LittleEngine
