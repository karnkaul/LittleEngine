#include "stdafx.h"
#include "AsyncAssetLoader.h"
#include "EngineRepository.h"
#include "LittleEngine/Services/Services.h"
#include "SFMLAPI/System/SFAssets.h"

namespace LittleEngine
{
using Lock = std::lock_guard<std::mutex>;

AsyncAssetLoader::AsyncAssetLoader(EngineRepository& repository, const String& manifestPath, Function(void()) onDone)
	: m_onDone(onDone), m_pRepository(&repository)
{
	AssetManifestData data(manifestPath);
	AssetManifest& manifest = data.GetManifest();

	for (auto& definition : manifest.definitions)
	{
		switch (definition.type)
		{
		case AssetType::Texture:
		{
			AddTexturePaths(definition.resourcePaths);
			break;
		}

		case AssetType::Font:
		{
			AddFontPaths(definition.resourcePaths);
			break;
		}

		case AssetType::Sound:
		{
			AddSoundPaths(definition.resourcePaths);
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
				sound.asset = m_pRepository->LoadInternal<SoundAsset>(sound.assetPath);
			}
		},
		"Load All Sounds");

	for (auto& texture : m_newTextures)
	{
		m_pMultiJob->AddJob(
			[&]() { texture.asset = m_pRepository->LoadInternal<TextureAsset>(texture.assetPath); },
			texture.assetPath);
	}
	for (auto& font : m_newFonts)
	{
		m_pMultiJob->AddJob(
			[&]() { font.asset = m_pRepository->LoadInternal<FontAsset>(font.assetPath); }, font.assetPath);
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
				m_pRepository->m_loaded.emplace(newAsset.assetPath, std::move(newAsset.asset));
			}
			for (auto& newAsset : m_newFonts)
			{
				m_pRepository->m_loaded.emplace(newAsset.assetPath, std::move(newAsset.asset));
			}
			for (auto& newAsset : m_newSounds)
			{
				m_pRepository->m_loaded.emplace(newAsset.assetPath, std::move(newAsset.asset));
			}
		}
		if (m_onDone)
			m_onDone();
		m_onDone = nullptr;
		m_bIdle = true;
	}
}

void AsyncAssetLoader::AddTexturePaths(const AssetPaths& paths)
{
	for (auto& path : paths.assetPaths)
	{
		String fullPath = m_pRepository->GetPath(path);
		if (!m_pRepository->IsLoaded(fullPath))
		{
			m_newTextures.emplace_back(fullPath);
		}
	}
}

void AsyncAssetLoader::AddFontPaths(const AssetPaths& paths)
{
	for (auto& path : paths.assetPaths)
	{
		String fullPath = m_pRepository->GetPath(path);
		if (!m_pRepository->IsLoaded(fullPath))
		{
			m_newFonts.emplace_back(fullPath);
		}
	}
}

void AsyncAssetLoader::AddSoundPaths(const AssetPaths& paths)
{
	for (auto& path : paths.assetPaths)
	{
		String fullPath = m_pRepository->GetPath(path);
		if (!m_pRepository->IsLoaded(fullPath))
		{
			m_newSounds.emplace_back(fullPath);
		}
	}
}
} // namespace LittleEngine
