#include "stdafx.h"
#include <fstream>
#include "Core/OS.h"
#include "LERepository.h"
#include "AssetManifest.h"
#include "ManifestLoader.h"
#include "LittleEngine/FatalEngineException.h"

namespace LittleEngine
{
LERepository* g_pRepository = nullptr;

LERepository::LERepository(String defaultFontID, String archivePath, String rootDir)
	: m_rootDir(std::move(rootDir))
{
	m_assetPathPrefix = m_rootDir.empty() ? String() : m_rootDir + "/";
	m_uCooked = MakeUnique<Core::ArchiveReader>();
	std::ifstream file(archivePath);
	Assert(file.good(), "Cooked archive does not exist!");
#if DEBUGGING
	if (OS::IsDebuggerAttached())
	{
		// If you are debugging and have broken here,
		// no need to restart the session, just fix the
		// missing archive before continuing!
		file = std::ifstream(archivePath);
	}
#endif
	if (!file.good())
	{
		LOG_E("Cooked archive [%s] does not exist!", archivePath.c_str());
#if !ENABLED(FILESYSTEM_ASSETS)
		throw FatalEngineException();
#endif
	}
	else
	{
		m_uCooked->Load(archivePath.c_str());
	}
	LOG_D("[Repository] Located cooked archive at [%s]", archivePath.c_str());

	if (!m_uCooked->IsPresent(defaultFontID.c_str()))
	{
		LOG_E("[Repository] Cooked assets does not contain %s!", defaultFontID.c_str());
	}
	else
	{
		UPtr<FontAsset> uDefaultFont = CreateAsset<FontAsset>(defaultFontID, m_uCooked->Decompress(defaultFontID.c_str()));
		if (uDefaultFont)
		{
			m_pDefaultFont = uDefaultFont.get();
			m_loaded.emplace(defaultFontID, std::move(uDefaultFont));
		}
		else
		{
			LOG_E("[Repository] Could not load %s from Cooked assets!", defaultFontID.c_str());
		}
	}

#if ENABLED(FILESYSTEM_ASSETS)
	if (!m_pDefaultFont)
	{
		UPtr<FontAsset> uDefaultFont = RetrieveAsset<FontAsset>(defaultFontID);
		if (uDefaultFont)
		{
			m_pDefaultFont = uDefaultFont.get();
			m_loaded.emplace(defaultFontID, std::move(uDefaultFont));
		}
		else
		{
			LOG_E("[Repository] Could not load %s from filesystem assets!", defaultFontID.c_str());
		}
	}
#endif
	Assert(m_pDefaultFont, "Invariant violated: Default Font is null!");

	g_pRepository = this;
	LOG_D("[Repository] constructed");
}

FontAsset* LERepository::GetDefaultFont() const
{
	Assert(m_pDefaultFont, "Default Font has not been set!");
	return m_pDefaultFont;
}

ManifestLoader* LERepository::LoadAsync(String manifestPath, Task onComplete)
{
	UPtr<ManifestLoader> uAsyncLoader =
		MakeUnique<ManifestLoader>(*this, std::move(manifestPath), std::move(onComplete));
	ManifestLoader* pLoader = uAsyncLoader.get();
	m_uAsyncLoaders.emplace_back(std::move(uAsyncLoader));
	return pLoader;
}

bool LERepository::IsLoaded(const String& id) const
{
	Lock lock(m_mutex);
	return m_loaded.find(id) != m_loaded.end();
}

u64 LERepository::GetLoadedBytes() const
{
	Lock lock(m_mutex);
	u64 total = 0;
	for (auto& uAsset : m_loaded)
	{
		total += uAsset.second->GetByteCount();
	}
	return total;
}

bool LERepository::Unload(String id)
{
	bool bPresent = m_loaded.find(id) != m_loaded.end();
	if (bPresent)
	{
		Lock lock(m_mutex);
		m_loaded.erase(id);
	}
	return bPresent;
}

void LERepository::UnloadAll(bool bUnloadDefaultFont)
{
	if (bUnloadDefaultFont || !m_pDefaultFont)
	{
		m_loaded.clear();
	}
	else
	{
		String fontID = m_pDefaultFont->GetID();
		Core::RemoveIf<String, UPtr<Asset>>(
			m_loaded, [fontID](UPtr<Asset>& uAsset) { return uAsset->GetID() != fontID; });
	}
	LOG_D("[Repository] cleared");
}

bool LERepository::IsBusy() const
{
	return !m_uAsyncLoaders.empty();
}

void LERepository::Tick(Time dt)
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

LERepository::~LERepository()
{
	g_pRepository = nullptr;
	m_pDefaultFont = nullptr;
	m_loaded.clear();
	LOG_D("[Repository] destroyed");
}

#if ENABLED(FILESYSTEM_ASSETS)
bool LERepository::DoesFileAssetExist(const String& id)
{
	std::ifstream file(GetFileAssetPath(id));
	return file.good();
}
String LERepository::GetFileAssetPath(const String& id) const
{
	return m_assetPathPrefix + id;
}
#endif
} // namespace LittleEngine
