#include <fstream>
#include "Core/Jobs.h"
#include "Core/ArchiveReader.h"
#include "SFMLAPI/System/Assets.h"
#include "LERepository.h"
#include "ManifestLoader.h"
#include "Engine/Rendering/LEShaders.h"

namespace LittleEngine
{
ManifestLoader::ManifestLoader(LERepository& repository, std::string manifestPath, Task onDone, bool bUnload)
	: m_onDone(std::move(onDone)), m_manifestPath(std::move(manifestPath)), m_pRepository(&repository), m_bUnloading(bUnload)
{
	AssetManifestData data;
#if ENABLED(FILESYSTEM_ASSETS)
	std::ifstream fileManifest(Asset::FilePathAndSize(m_manifestPath).first.c_str());
	m_bManifestFilePresent = fileManifest.good();
	if (!m_bManifestFilePresent)
	{
		LOG_W("[ManifestLoader] FILESYSTEM_ASSETS enabled but %s missing from filesystem! Loading from cooked assets",
			  m_manifestPath.c_str());
	}
#endif
	TextAsset* pManifest = nullptr;
	if (repository.IsLoaded(m_manifestPath))
	{
		pManifest = repository.GetLoaded<TextAsset>(m_manifestPath);
	}
	else
	{
		auto uManifest =
			repository.ConjureAsset<TextAsset>(m_manifestPath, {LERepository::Search::Filesystem, LERepository::Search::Cooked});
		pManifest = uManifest.get();
		if (uManifest)
		{
			repository.m_loaded.emplace(m_manifestPath, std::move(uManifest));
		}
	}

	if (pManifest)
	{
		data.Deserialise(pManifest->Text());
	}

	AssetManifest& manifest = data.Manifest();
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
			LOG_W("[ManifestLoader] Unsupported Asset Type [%s] for asynchronous loading!", g_szAssetType[ToIdx(definition.type)].data());
		}
	}

	m_bCompleted = m_bIdle = false;

	if (m_bUnloading)
	{
		Lock lock(m_pRepository->m_loadedMutex);
		for (auto& newAsset : m_newTextures)
		{
			m_pRepository->m_loaded.erase(newAsset.assetID);
		}
		for (auto& newAsset : m_newFonts)
		{
			m_pRepository->m_loaded.erase(newAsset.assetID);
		}
		for (auto& newAsset : m_newSounds)
		{
			m_pRepository->m_loaded.erase(newAsset.assetID);
		}
		for (auto& newAsset : m_newTexts)
		{
			m_pRepository->m_loaded.erase(newAsset.assetID);
		}
		m_bCompleted = true;
	}
	else
	{
#if ENABLED(FILESYSTEM_ASSETS)
		bool bUsingFileSystem = false;
		bUsingFileSystem = m_bManifestFilePresent && LERepository::s_bUseFileAssets;
		if (bUsingFileSystem)
		{
			// Load
			m_pJobCatalog = Core::Jobs::CreateCatalog(m_manifestPath + "(FSLoad)");
			m_pJobCatalog->AddJob(
				[&]() {
					for (auto& sound : m_newSounds)
					{
						sound.asset = m_pRepository->CreateAsset<SoundAsset>(sound.assetID);
					}
				},
				"Load All Sounds");

			for (auto& tx : m_newTextures)
			{
				m_pJobCatalog->AddJob([&]() { tx.asset = m_pRepository->CreateAsset<TextureAsset>(tx.assetID); }, tx.assetID);
			}
			for (auto& font : m_newFonts)
			{
				m_pJobCatalog->AddJob([&]() { font.asset = m_pRepository->CreateAsset<FontAsset>(font.assetID); }, font.assetID);
			}
			for (auto& text : m_newTexts)
			{
				m_pJobCatalog->AddJob([&]() { text.asset = m_pRepository->CreateAsset<TextAsset>(text.assetID); }, text.assetID);
			}
		}
		else
#endif
		{
			m_pJobCatalog = Core::Jobs::CreateCatalog(m_manifestPath + "(Decompress)");
			m_pJobCatalog->AddJob(
				[&]() {
					for (auto& sound : m_newSounds)
					{
						sound.asset = m_pRepository->CreateAsset<SoundAsset>(sound.assetID,
																			 m_pRepository->m_uCooked->Decompress(sound.assetID.c_str()));
					}
				},
				"Load All Sounds");

			for (auto& tx : m_newTextures)
			{
				m_pJobCatalog->AddJob(
					[&]() {
						tx.asset =
							m_pRepository->CreateAsset<TextureAsset>(tx.assetID, m_pRepository->m_uCooked->Decompress(tx.assetID.c_str()));
					},
					tx.assetID);
			}
			for (auto& font : m_newFonts)
			{
				m_pJobCatalog->AddJob(
					[&]() {
						font.asset =
							m_pRepository->CreateAsset<FontAsset>(font.assetID, m_pRepository->m_uCooked->Decompress(font.assetID.c_str()));
					},
					font.assetID);
			}
			for (auto& text : m_newTexts)
			{
				m_pJobCatalog->AddJob(
					[&]() {
						text.asset =
							m_pRepository->CreateAsset<TextAsset>(text.assetID, m_pRepository->m_uCooked->Decompress(text.assetID.c_str()));
					},
					text.assetID);
			}
		}
		m_pJobCatalog->StartJobs([&]() { m_bCompleted = true; });
	}
}

Fixed ManifestLoader::Progress() const
{
	return m_pJobCatalog ? m_pJobCatalog->Progress() : -Fixed::One;
}

void ManifestLoader::Tick(Time /*dt*/)
{
	if (m_bCompleted)
	{
		if (!m_bUnloading)
		{
			Lock lock(m_pRepository->m_loadedMutex);
			for (auto& newAsset : m_newTextures)
			{
				if (newAsset.asset)
				{
					m_pRepository->m_loaded[std::string(newAsset.asset->ID())] = std::move(newAsset.asset);
				}
			}
			for (auto& newAsset : m_newFonts)
			{
				if (newAsset.asset)
				{
					m_pRepository->m_loaded[std::string(newAsset.asset->ID())] = std::move(newAsset.asset);
				}
			}
			for (auto& newAsset : m_newSounds)
			{
				if (newAsset.asset)
				{
					m_pRepository->m_loaded[std::string(newAsset.asset->ID())] = std::move(newAsset.asset);
				}
			}
			for (auto& newAsset : m_newTexts)
			{
				if (newAsset.asset)
				{
					m_pRepository->m_loaded[std::string(newAsset.asset->ID())] = std::move(newAsset.asset);
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
		if (!(m_pRepository->IsLoaded(id) ^ m_bUnloading))
		{
			m_newTextures.emplace_back(id);
		}
	}
}

void ManifestLoader::AddFontIDs(AssetIDContainer IDs)
{
	for (const auto& id : IDs.assetIDs)
	{
		if (!(m_pRepository->IsLoaded(id) ^ m_bUnloading))
		{
			m_newFonts.emplace_back(id);
		}
	}
}

void ManifestLoader::AddSoundIDs(AssetIDContainer IDs)
{
	for (const auto& id : IDs.assetIDs)
	{
		if (!(m_pRepository->IsLoaded(id) ^ m_bUnloading))
		{
			m_newSounds.emplace_back(id);
		}
	}
}

void ManifestLoader::AddTextIDs(AssetIDContainer IDs)
{
	for (const auto& id : IDs.assetIDs)
	{
		if (!(m_pRepository->IsLoaded(id) ^ m_bUnloading))
		{
			m_newTexts.emplace_back(id);
		}
	}
}
} // namespace LittleEngine
