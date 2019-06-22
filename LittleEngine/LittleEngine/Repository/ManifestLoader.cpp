#include "stdafx.h"
#include "Core/Jobs.h"
#include "Core/ArchiveReader.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LERepository.h"
#include "ManifestLoader.h"
#include "LittleEngine/Renderer/ShaderRepository.h"

namespace LittleEngine
{
ManifestLoader::ManifestLoader(LERepository& repository, String manifestPath, Task onDone)
	: m_onDone(std::move(onDone)), m_pRepository(&repository)
{
	AssetManifestData data;
#if ENABLED(FILESYSTEM_ASSETS)
	std::ifstream fileManifest(m_pRepository->GetFileAssetPath(manifestPath));
	m_bManifestFilePresent = fileManifest.good();
	if (m_bManifestFilePresent)
	{
		LOG_D("[ManifestLoader] Loading [%s] from filesystem", manifestPath.c_str());
		data.Load(m_pRepository->GetFileAssetPath(manifestPath));
	}
	else
	{
		LOG_W("[ManifestLoader] FILESYSTEM_ASSETS enabled but %s missing from filesystem! Loading from cooked assets",
			  manifestPath.c_str());
		String manifestText =
			Core::ArchiveReader::ToText(m_pRepository->m_uCooked->Decompress(manifestPath.c_str()));
		data.Deserialise(std::move(manifestText));
	}
#else
	LOG_D("[ManifestLoader] Decompressing [%s] from cooked assets", manifestPath.c_str());
	String manifestText =
		Core::ArchiveReader::ToText(m_pRepository->m_uCooked->Decompress(manifestPath.c_str()));
	data.Deserialise(std::move(manifestText));
#endif

	AssetManifest& manifest = data.GetManifest();

#if ENABLED(FILESYSTEM_ASSETS)
	// Verify
	for (const auto& definition : manifest.assetDefs)
	{
		for (const auto& id : definition.assetIDs.assetIDs)
		{
			if (!m_pRepository->m_uCooked->IsPresent(id.c_str()))
			{
				LOG_W("[ManifestLoader] Asset not present in cooked archive! [%s]", id.c_str());
			}
		}
	}
#endif

	for (auto& definition : manifest.assetDefs)
	{
		switch (definition.type)
		{
		case AssetType::Texture:
		{
			AddTextureIDs(std::move(definition.assetIDs));
			break;
		}

		case AssetType::Font:
		{
			AddFontIDs(std::move(definition.assetIDs));
			break;
		}

		case AssetType::Sound:
		{
			AddSoundIDs(std::move(definition.assetIDs));
			break;
		}

		case AssetType::Text:
		{
			AddTextIDs(std::move(definition.assetIDs));
			break;
		}

		default:
			LOG_W("[ManifestLoader] Unsupported Asset Type [%s] for asynchronous loading!",
				  g_szAssetType[ToIdx(definition.type)]);
		}
	}

	m_bCompleted = m_bIdle = false;

#if ENABLED(FILESYSTEM_ASSETS)
	bool bUsingFileSystem = false;
	bUsingFileSystem = m_bManifestFilePresent;
	if (bUsingFileSystem)
	{
		// Load
		m_pJobCatalog = Core::Jobs::CreateCatalog(manifestPath + "(FSLoad)");
		m_pJobCatalog->AddJob(
			[&]() {
				for (auto& sound : m_newSounds)
				{
					sound.asset = m_pRepository->RetrieveAsset<SoundAsset>(sound.assetID);
				}
			},
			"Load All Sounds");

		for (auto& texture : m_newTextures)
		{
			m_pJobCatalog->AddJob(
				[&]() {
					texture.asset = m_pRepository->RetrieveAsset<TextureAsset>(texture.assetID);
				},
				texture.assetID);
		}
		for (auto& font : m_newFonts)
		{
			m_pJobCatalog->AddJob(
				[&]() { font.asset = m_pRepository->RetrieveAsset<FontAsset>(font.assetID); }, font.assetID);
		}
		for (auto& text : m_newTexts)
		{
			m_pJobCatalog->AddJob(
				[&]() { text.asset = m_pRepository->RetrieveAsset<TextAsset>(text.assetID); }, text.assetID);
		}
	}
	else
#endif
	{
		m_pJobCatalog = Core::Jobs::CreateCatalog(manifestPath + "(Decompress)");
		m_pJobCatalog->AddJob(
			[&]() {
				for (auto& sound : m_newSounds)
				{
					sound.asset = m_pRepository->CreateAsset<SoundAsset>(
						sound.assetID, m_pRepository->m_uCooked->Decompress(sound.assetID.c_str()));
				}
			},
			"Load All Sounds");

		for (auto& texture : m_newTextures)
		{
			m_pJobCatalog->AddJob(
				[&]() {
					texture.asset = m_pRepository->CreateAsset<TextureAsset>(
						texture.assetID, m_pRepository->m_uCooked->Decompress(texture.assetID.c_str()));
				},
				texture.assetID);
		}
		for (auto& font : m_newFonts)
		{
			m_pJobCatalog->AddJob(
				[&]() {
					font.asset = m_pRepository->CreateAsset<FontAsset>(
						font.assetID, m_pRepository->m_uCooked->Decompress(font.assetID.c_str()));
				},
				font.assetID);
		}
		for (auto& text : m_newTexts)
		{
			m_pJobCatalog->AddJob(
				[&]() {
					text.asset = m_pRepository->CreateAsset<TextAsset>(
						text.assetID, m_pRepository->m_uCooked->Decompress(text.assetID.c_str()));
				},
				text.assetID);
		}
	}
	m_pJobCatalog->StartJobs([&]() { m_bCompleted = true; });
}

Fixed ManifestLoader::GetProgress() const
{
	return m_pJobCatalog ? m_pJobCatalog->GetProgress() : -Fixed::One;
}

void ManifestLoader::Tick(Time /*dt*/)
{
	if (m_bCompleted)
	{
		{
			Lock lock(m_pRepository->m_mutex);
			for (auto& newAsset : m_newTextures)
			{
				if (newAsset.asset)
				{
					m_pRepository->m_loaded.emplace(newAsset.assetID, std::move(newAsset.asset));
				}
			}
			for (auto& newAsset : m_newFonts)
			{
				if (newAsset.asset)
				{
					m_pRepository->m_loaded.emplace(newAsset.assetID, std::move(newAsset.asset));
				}
			}
			for (auto& newAsset : m_newSounds)
			{
				if (newAsset.asset)
				{
					m_pRepository->m_loaded.emplace(newAsset.assetID, std::move(newAsset.asset));
				}
			}
			for (auto& newAsset : m_newTexts)
			{
				if (newAsset.asset)
				{
					m_pRepository->m_loaded.emplace(newAsset.assetID, std::move(newAsset.asset));
				}
			}
		}
		if (m_onDone)
		{
			m_onDone();
		}
		m_onDone = nullptr;
		m_bIdle = true;
	}
}

void ManifestLoader::AddTextureIDs(AssetIDContainer IDs)
{
	for (const auto& id : IDs.assetIDs)
	{
		if (!m_pRepository->IsLoaded(id))
		{
			m_newTextures.emplace_back(id);
		}
	}
}

void ManifestLoader::AddFontIDs(AssetIDContainer IDs)
{
	for (const auto& id : IDs.assetIDs)
	{
		if (!m_pRepository->IsLoaded(id))
		{
			m_newFonts.emplace_back(id);
		}
	}
}

void ManifestLoader::AddSoundIDs(AssetIDContainer IDs)
{
	for (const auto& id : IDs.assetIDs)
	{
		if (!m_pRepository->IsLoaded(id))
		{
			m_newSounds.emplace_back(id);
		}
	}
}

void ManifestLoader::AddTextIDs(AssetIDContainer IDs)
{
	for (const auto& id : IDs.assetIDs)
	{
		if (!m_pRepository->IsLoaded(id))
		{
			m_newTexts.emplace_back(id);
		}
	}
}
} // namespace LittleEngine
