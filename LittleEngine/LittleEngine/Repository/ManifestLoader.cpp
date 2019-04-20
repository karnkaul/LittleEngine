#include "stdafx.h"
#include "ArchiveReader.h"
#include "SFMLAPI/System/SFAssets.h"
#include "ManifestLoader.h"
#include "EngineRepository.h"
#include "LittleEngine/Jobs/JobManager.h"
#include "LittleEngine/Jobs/MultiJob.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
using Lock = std::lock_guard<std::mutex>;

#if !SHIPPING
ManifestLoader::ManifestLoader(EngineRepository& repository,
								   String manifestPath,
								   std::function<void()> onDone)
	: m_onDone(std::move(onDone)), m_pRepository(&repository)
{
	AssetManifestData data;
	data.Load(std::move(manifestPath));
	AssetManifest& manifest = data.GetManifest();
	for (auto& definition : manifest.definitions)
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
			LOG_W("Unsupported Asset Type [%d] for asynchronous loading!", definition.type);
		}
	}

	m_bCompleted = m_bIdle = false;
	m_pMultiJob = Services::Jobs()->CreateMultiJob("Load " + manifestPath);
	m_pMultiJob->AddJob(
		[&]() {
			for (auto& sound : m_newSounds)
			{
				sound.asset = m_pRepository->FetchAsset<SoundAsset>(sound.assetID);
			}
		},
		"Load All Sounds");

	for (auto& texture : m_newTextures)
	{
		m_pMultiJob->AddJob(
			[&]() { texture.asset = m_pRepository->FetchAsset<TextureAsset>(texture.assetID); },
			texture.assetID);
	}
	for (auto& font : m_newFonts)
	{
		m_pMultiJob->AddJob(
			[&]() { font.asset = m_pRepository->FetchAsset<FontAsset>(font.assetID); }, font.assetID);
	}
	for (auto& text : m_newTexts)
	{
		m_pMultiJob->AddJob(
			[&]() { text.asset = m_pRepository->FetchAsset<TextAsset>(text.assetID); }, text.assetID);
	}

	m_pMultiJob->StartJobs([&]() { m_bCompleted = true; });
}
#endif

ManifestLoader::ManifestLoader(EngineRepository& repository,
								   String archivePath,
								   String manifestPath,
								   std::function<void()> onDone)
	: m_onDone(std::move(onDone)), m_pRepository(&repository)
{
	m_uArchiveReader = MakeUnique<Core::ArchiveReader>();
	m_uArchiveReader->Load(archivePath.c_str());
	String manifestText = m_uArchiveReader->ToText(m_uArchiveReader->Decompress(manifestPath.c_str()));
	AssetManifestData data;
	data.Deserialise(std::move(manifestText));

	AssetManifest& manifest = data.GetManifest();
	for (auto& definition : manifest.definitions)
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
			LOG_W("Unsupported Asset Type [%d] for asynchronous loading!", definition.type);
		}
	}

	m_bCompleted = m_bIdle = false;
	m_pMultiJob = Services::Jobs()->CreateMultiJob("Decompress " + archivePath);
	m_pMultiJob->AddJob(
		[&]() {
			for (auto& sound : m_newSounds)
			{
				sound.asset = m_pRepository->CreateAsset<SoundAsset>(
					sound.assetID, m_uArchiveReader->Decompress(sound.assetID.c_str()));
			}
		},
		"Load All Sounds");

	for (auto& texture : m_newTextures)
	{
		m_pMultiJob->AddJob(
			[&]() {
				texture.asset = m_pRepository->CreateAsset<TextureAsset>(
					texture.assetID, m_uArchiveReader->Decompress(texture.assetID.c_str()));
			},
			texture.assetID);
	}
	for (auto& font : m_newFonts)
	{
		m_pMultiJob->AddJob(
			[&]() {
				font.asset = m_pRepository->CreateAsset<FontAsset>(
					font.assetID, m_uArchiveReader->Decompress(font.assetID.c_str()));
			},
			font.assetID);
	}
	for (auto& text : m_newTexts)
	{
		m_pMultiJob->AddJob(
			[&]() {
				text.asset = m_pRepository->CreateAsset<TextAsset>(
					text.assetID, m_uArchiveReader->Decompress(text.assetID.c_str()));
			},
			text.assetID);
	}

	m_pMultiJob->StartJobs([&]() { m_bCompleted = true; });
}

Fixed ManifestLoader::GetProgress() const
{
	return m_pMultiJob ? m_pMultiJob->GetProgress() : -Fixed::One;
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
