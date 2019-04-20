#include "stdafx.h"
#include "ArchiveReader.h"
#include "Utils.h"
#include "SFMLAPI/System/SFAssets.h"
#include "EngineRepository.h"
#include "LoadHelpers.h"
#include "ManifestLoader.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Jobs/JobManager.h"

namespace LittleEngine
{
EngineRepository::EngineRepository(String archivePath, String rootDir)
	: m_rootDir(std::move(rootDir)), m_pDefaultFont(nullptr)
{
	m_uCooked = MakeUnique<Core::ArchiveReader>();
	String fontID = "Fonts/main.ttf";
	m_uCooked->Load(archivePath.c_str());
	if (!m_uCooked->IsPresent(fontID.c_str()))
	{
		LOG_E("[EngineRepository] Cooked assets does not contain %s!", fontID.c_str());
	}
	else
	{
		UPtr<FontAsset> uDefaultFont = CreateAsset<FontAsset>(fontID, m_uCooked->Decompress(fontID.c_str()));
		if (uDefaultFont)
		{
			m_pDefaultFont = uDefaultFont.get();
			m_loaded.emplace(fontID, std::move(uDefaultFont));
		}
		else
		{
			LOG_E("[EngineRepository] Could not load %s from Cooked assets!", fontID.c_str());
		}
	}

#if !SHIPPING
	if (!m_pDefaultFont)
	{
		UPtr<FontAsset> uDefaultFont = FetchAsset<FontAsset>(fontID);
		if (uDefaultFont)
		{
			m_pDefaultFont = uDefaultFont.get();
			m_loaded.emplace(fontID, std::move(uDefaultFont));
		}
		else
		{
			LOG_E("[EngineRepository] Could not load %s from filesystem assets!", fontID.c_str());
		}
	}
#endif
	LOG_I("[EngineRepository] constructed");
	Assert(m_pDefaultFont, "Invariant violated: Default Font is null!");
}

FontAsset* EngineRepository::GetDefaultFont() const
{
	return m_pDefaultFont;
}

#if !SHIPPING
ManifestLoader* EngineRepository::LoadAsync(String manifestPath, std::function<void()> onComplete)
{
	UPtr<ManifestLoader> uAsyncLoader = MakeUnique<ManifestLoader>(*this, std::move(manifestPath), std::move(onComplete));
	ManifestLoader* pLoader = uAsyncLoader.get();
	m_uAsyncLoaders.emplace_back(std::move(uAsyncLoader));
	return pLoader;
}
#endif

ManifestLoader* EngineRepository::LoadAsync(String archivePath,
											  String manifestPath,
											  std::function<void()> onComplete)
{
	UPtr<ManifestLoader> uAsyncLoader =
		MakeUnique<ManifestLoader>(*this, std::move(archivePath), std::move(manifestPath), std::move(onComplete));
	ManifestLoader* pLoader = uAsyncLoader.get();
	m_uAsyncLoaders.emplace_back(std::move(uAsyncLoader));
	return pLoader;
}

bool EngineRepository::Unload(String id)
{
	bool bPresent = m_loaded.find(id) != m_loaded.end();
	if (bPresent)
	{
		Services::Jobs()->Enqueue([&, id]() { 
			Lock lock(m_mutex);
			m_loaded.erase(id);
		}, "", true);
	}
	return bPresent;
	//return m_loaded.erase(id);
}

void EngineRepository::UnloadAll(bool bUnloadDefaultFont)
{
	if (bUnloadDefaultFont || !m_pDefaultFont)
	{
		m_loaded.clear();
	}
	else
	{
		String fontID = m_pDefaultFont->GetID();
		Core::CleanMap<String, UPtr<Asset>>(
			m_loaded, [fontID](UPtr<Asset>& uAsset) { return uAsset->GetID() != fontID; });
	}
	LOG_I("[EngineRepository] cleared");
}

bool EngineRepository::IsLoaded(const String& id)
{
	auto search = m_loaded.find(id);
	return search != m_loaded.end();
}

void EngineRepository::Tick(Time dt)
{
	auto iter = m_uAsyncLoaders.begin();
	while (iter != m_uAsyncLoaders.end())
	{
		(*iter)->Tick(dt);
		if ((*iter)->m_bIdle)
		{
			iter = m_uAsyncLoaders.erase(iter);
			continue;
		}
		++iter;
	}
}

EngineRepository::~EngineRepository()
{
	m_pDefaultFont = nullptr;
	UnloadAll(true);
	Core::ArchiveReader::UnInit();
	LOG_I("[EngineRepository] destroyed");
}
} // namespace LittleEngine
