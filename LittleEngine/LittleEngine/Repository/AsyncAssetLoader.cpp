#include "stdafx.h"
#include "AsyncAssetLoader.h"
#include "EngineRepository.h"
#include "LittleEngine/Services/Services.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LittleEngine/Jobs/JobManager.h"
#include "LittleEngine/Jobs/MultiJob.h"

namespace LittleEngine
{
using Lock = std::lock_guard<std::mutex>;

#if !SHIPPING
AsyncAssetLoader::AsyncAssetLoader(EngineRepository& repository,
								   const String& manifestPath,
								   const std::function<void()>& onDone)
	: m_onDone(onDone), m_pRepository(&repository)
{
	AssetManifestData data;
	data.Load(manifestPath);
	AssetManifest& manifest = data.GetManifest();
	for (auto& definition : manifest.definitions)
	{
		switch (definition.type)
		{
		case AssetType::Texture:
		{
			AddTextureIDs(definition.assetIDs);
			break;
		}

		case AssetType::Font:
		{
			AddFontIDs(definition.assetIDs);
			break;
		}

		case AssetType::Sound:
		{
			AddSoundIDs(definition.assetIDs);
			break;
		}

		case AssetType::Text:
		{
			AddTextIDs(definition.assetIDs);
			break;
		}

		default:
			LOG_W("Unsupported Asset Type [%d] for asynchronous loading!", definition.type);
		}
	}

	m_bCompleted = m_bIdle = false;
	m_pMultiJob = Services::Jobs()->CreateMultiJob("Async Load: " + manifestPath);
	m_pMultiJob->AddJob(
		[&]() {
			for (auto& sound : m_newSounds)
			{
				sound.asset = m_pRepository->LoadInternal<SoundAsset>(sound.assetID);
			}
		},
		"Load All Sounds");

	for (auto& texture : m_newTextures)
	{
		m_pMultiJob->AddJob(
			[&]() { texture.asset = m_pRepository->LoadInternal<TextureAsset>(texture.assetID); },
			texture.assetID);
	}
	for (auto& font : m_newFonts)
	{
		m_pMultiJob->AddJob(
			[&]() { font.asset = m_pRepository->LoadInternal<FontAsset>(font.assetID); }, font.assetID);
	}
	for (auto& text : m_newTexts)
	{
		m_pMultiJob->AddJob(
			[&]() { text.asset = m_pRepository->LoadInternal<TextAsset>(text.assetID); }, text.assetID);
	}

	m_pMultiJob->StartJobs([&]() { m_bCompleted = true; });
}
#endif

AsyncAssetLoader::AsyncAssetLoader(EngineRepository& repository,
								   const String& archivePath,
								   const String& manifestPath,
								   const std::function<void()>& onDone)
	: m_onDone(onDone), m_pRepository(&repository)
{
	m_archiveReader.Load(archivePath.c_str());
	Vec<u8> manifestBuffer = m_archiveReader.Decompress(manifestPath);
	String manifestText = m_archiveReader.ToText(manifestBuffer);
	AssetManifestData data;
	data.Deserialise(manifestText);

	AssetManifest& manifest = data.GetManifest();
	for (auto& definition : manifest.definitions)
	{
		switch (definition.type)
		{
		case AssetType::Texture:
		{
			AddTextureIDs(definition.assetIDs);
			break;
		}

		case AssetType::Font:
		{
			AddFontIDs(definition.assetIDs);
			break;
		}

		case AssetType::Sound:
		{
			AddSoundIDs(definition.assetIDs);
			break;
		}

		case AssetType::Text:
		{
			AddTextIDs(definition.assetIDs);
			break;
		}

		default:
			LOG_W("Unsupported Asset Type [%d] for asynchronous loading!", definition.type);
		}
	}

	m_bCompleted = m_bIdle = false;
	m_pMultiJob = Services::Jobs()->CreateMultiJob("Async Decompress: " + archivePath);
	m_pMultiJob->AddJob(
		[&]() {
			for (auto& sound : m_newSounds)
			{
				sound.asset = m_pRepository->LoadInternal<SoundAsset>(
					sound.assetID, m_archiveReader.Decompress(sound.assetID));
			}
		},
		"Load All Sounds");

	for (auto& texture : m_newTextures)
	{
		m_pMultiJob->AddJob(
			[&]() {
				texture.asset = m_pRepository->LoadInternal<TextureAsset>(
					texture.assetID, m_archiveReader.Decompress(texture.assetID));
			},
			texture.assetID);
	}
	for (auto& font : m_newFonts)
	{
		m_pMultiJob->AddJob(
			[&]() {
				font.asset = m_pRepository->LoadInternal<FontAsset>(
					font.assetID, m_archiveReader.Decompress(font.assetID));
			},
			font.assetID);
	}
	for (auto& text : m_newTexts)
	{
		m_pMultiJob->AddJob(
			[&]() {
				text.asset = m_pRepository->LoadInternal<TextAsset>(
					text.assetID, m_archiveReader.Decompress(text.assetID));
			},
			text.assetID);
	}

	m_pMultiJob->StartJobs([&]() { m_bCompleted = true; });
}

Fixed AsyncAssetLoader::GetProgress() const
{
	return m_pMultiJob ? m_pMultiJob->GetProgress() : -1.0f;
}

void AsyncAssetLoader::Tick(Time)
{
	if (m_bCompleted)
	{
		{
			Lock lock(m_pRepository->m_mutex);
			for (auto& newAsset : m_newTextures)
			{
				if (newAsset.asset)
					m_pRepository->m_loaded.emplace(newAsset.assetID, std::move(newAsset.asset));
			}
			for (auto& newAsset : m_newFonts)
			{
				if (newAsset.asset)
					m_pRepository->m_loaded.emplace(newAsset.assetID, std::move(newAsset.asset));
			}
			for (auto& newAsset : m_newSounds)
			{
				if (newAsset.asset)
					m_pRepository->m_loaded.emplace(newAsset.assetID, std::move(newAsset.asset));
			}
			for (auto& newAsset : m_newTexts)
			{
				if (newAsset.asset)
					m_pRepository->m_loaded.emplace(newAsset.assetID, std::move(newAsset.asset));
			}
		}
		if (m_onDone)
			m_onDone();
		m_onDone = nullptr;
		m_bIdle = true;
	}
}

void AsyncAssetLoader::AddTextureIDs(const AssetIDContainer& IDs)
{
	for (auto& id : IDs.assetIDs)
	{
		if (!m_pRepository->IsLoaded(id))
		{
			m_newTextures.emplace_back(id);
		}
	}
}

void AsyncAssetLoader::AddFontIDs(const AssetIDContainer& IDs)
{
	for (auto& id : IDs.assetIDs)
	{
		if (!m_pRepository->IsLoaded(id))
		{
			m_newFonts.emplace_back(id);
		}
	}
}

void AsyncAssetLoader::AddSoundIDs(const AssetIDContainer& IDs)
{
	for (auto& id : IDs.assetIDs)
	{
		if (!m_pRepository->IsLoaded(id))
		{
			m_newSounds.emplace_back(id);
		}
	}
}

void AsyncAssetLoader::AddTextIDs(const AssetIDContainer& IDs)
{
	for (auto& id : IDs.assetIDs)
	{
		if (!m_pRepository->IsLoaded(id))
		{
			m_newTexts.emplace_back(id);
		}
	}
}
} // namespace LittleEngine
