#include "stdafx.h"
#include "EngineRepository.h"
#include "ManifestLoader.h"
#include "LittleEngine/Services/Services.h"

namespace LittleEngine
{
EngineRepository::EngineRepository(const String& archivePath, const String& rootDir)
	: m_rootDir(rootDir)
{
	String fontID = "Fonts/main.ttf";
	m_cooked.Load(archivePath.c_str());
	if (!m_cooked.IsPresent(fontID.c_str()))
	{
		LOG_E("[EngineRepository] Cooked assets does not contain %s!", fontID.c_str());
	}
	else
	{
		UPtr<FontAsset> uDefaultFont = CreateAsset<FontAsset>(fontID, m_cooked.Decompress(fontID.c_str()));
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
ManifestLoader* EngineRepository::LoadAsync(const String& manifestPath, const std::function<void()>& onComplete)
{
	UPtr<ManifestLoader> uAsyncLoader = MakeUnique<ManifestLoader>(*this, manifestPath, onComplete);
	ManifestLoader* pLoader = uAsyncLoader.get();
	m_uAsyncLoaders.emplace_back(std::move(uAsyncLoader));
	return pLoader;
}
#endif

ManifestLoader* EngineRepository::LoadAsync(const String& archivePath,
											  const String& manifestPath,
											  const std::function<void()>& onComplete)
{
	UPtr<ManifestLoader> uAsyncLoader =
		MakeUnique<ManifestLoader>(*this, archivePath, manifestPath, onComplete);
	ManifestLoader* pLoader = uAsyncLoader.get();
	m_uAsyncLoaders.emplace_back(std::move(uAsyncLoader));
	return pLoader;
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
