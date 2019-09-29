#include <fstream>
#include "Core/OS.h"
#include "LERepository.h"
#include "AssetManifest.h"
#include "ManifestLoader.h"
#include "Engine/FatalEngineException.h"

namespace LittleEngine
{
LERepository* g_pRepository = nullptr;
FontAsset* g_pDefaultFont = nullptr;
#if ENABLED(FILESYSTEM_ASSETS)
bool LERepository::s_bUseFileAssets = true;
#endif

LERepository::LERepository(String defaultFontID, String archivePath, String rootDir) : m_rootDir(std::move(rootDir))
{
	if (!m_rootDir.empty())
	{
		m_rootDir += "/";
	}
	Asset::s_pathPrefix = m_rootDir;
	m_uCooked = MakeUnique<Core::ArchiveReader>();

	LOG_D("[Repository] Assets Root Dir: %s", m_rootDir.c_str());

	String filePath = OS::Env()->FullPath(archivePath);
	std::ifstream file(filePath.c_str());
	Assert(file.good(), "Cooked archive does not exist!");
#if defined(DEBUGGING)
	if (OS::IsDebuggerAttached())
	{
		// If you are debugging and have broken here,
		// no need to restart the session, just fix the
		// missing archive before continuing!
		file = std::ifstream(filePath.c_str());
	}
#endif
	if (!file.good())
	{
		LOG_E("[Repository] Cooked archive [%s] does not exist!", archivePath.c_str());
#if !ENABLED(FILESYSTEM_ASSETS)
		throw FatalEngineException();
#endif
	}
	else
	{
		LOG_D("[Repository] Located cooked archive at [%s]", archivePath.c_str());
		m_uCooked->Load(filePath.c_str());
	}

	LoadDefaultFont(std::move(defaultFontID));
	g_pRepository = this;
	LOG_D("[Repository] constructed");
}

LERepository::~LERepository()
{
	g_pRepository = nullptr;
	g_pDefaultFont = nullptr;
	m_loaded.clear();
	LOG_D("[Repository] destroyed");
}

void LERepository::LoadDefaultFont(String id)
{
	if (m_loaded.find(id) == m_loaded.end())
	{
		g_pDefaultFont = nullptr;
		auto uFont = ConjureAsset<FontAsset>(id, {Search::Filesystem, Search::Cooked});
		if (uFont)
		{
			g_pDefaultFont = uFont.get();
			m_loaded.emplace(std::move(id), std::move(uFont));
		}
	}
	Assert(g_pDefaultFont, "Invariant violated: Default Font is null!");
}

SPtr<ManifestLoader> LERepository::LoadManifest(String manifestPath, Task onComplete)
{
	SPtr<ManifestLoader> sLoader = MakeShared<ManifestLoader>(*this, std::move(manifestPath), std::move(onComplete), false);
	m_loaders.push_back(sLoader);
	return sLoader;
}

void LERepository::UnloadManifest(String manifestPath, Task onComplete /* = nullptr */)
{
	m_loaders.emplace_back(MakeUnique<ManifestLoader>(*this, std::move(manifestPath), std::move(onComplete), true));
}

bool LERepository::IsBusy() const
{
	for (auto& sLoader : m_loaders)
	{
		if (!sLoader->m_bUnloading)
		{
			return true;
		}
	}
	return false;
}

bool LERepository::IsLoaded(VString id) const
{
	Lock lock(m_loadedMutex);
	return m_loaded.find(id) != m_loaded.end();
}

u64 LERepository::LoadedBytes() const
{
	Lock lock(m_loadedMutex);
	u64 total = 0;
	for (auto& uAsset : m_loaded)
	{
		total += uAsset.second->ByteCount();
	}
	return total;
}

bool LERepository::IsPresent(VString id) const
{
	bool bRet = m_uCooked->IsPresent(id);
#if ENABLED(FILESYSTEM_ASSETS)
	bRet |= Asset::DoesFileExist(id);
#endif
	return bRet;
}

bool LERepository::Unload(VString id)
{
	bool bPresent = m_loaded.find(id) != m_loaded.end();
	if (bPresent)
	{
		Lock lock(m_loadedMutex);
		m_loaded.erase(id);
	}
	return bPresent;
}

void LERepository::UnloadAll(bool bUnloadDefaultFont)
{
	if (bUnloadDefaultFont || !g_pDefaultFont)
	{
		m_loaded.clear();
		g_pDefaultFont = nullptr;
	}
	else
	{
		Core::RemoveIf<VString, UPtr<Asset>>(m_loaded, [](UPtr<Asset>& uAsset) { return String(uAsset->ID()) != g_pDefaultFont->ID(); });
	}
	LOG_D("[Repository] cleared");
}

void LERepository::ResetState()
{
	m_state = State::Idle;
}

void LERepository::Tick(Time dt)
{
	m_state = State::Active;
	std::for_each(m_loaders.begin(), m_loaders.end(), [dt](auto& sLoader) { sLoader->Tick(dt); });
	Core::RemoveIf<SPtr<ManifestLoader>>(m_loaders, [](auto& sLoader) { return sLoader->m_bIdle; });
}

void LERepository::DoLoad(VString id, bool bSyncWarn, Task inCooked, 
#if ENABLED(FILESYSTEM_ASSETS)
	Task onFilesystem,
#endif
	Task onAssetMissing /* = nullptr */)
{
	LOGIF_W(bSyncWarn, "[Repository] Synchronously loading Asset (add id to manifest or use LoadAsync()) [%s]", id.data());
	bool bInCooked = m_uCooked->IsPresent(id);
#if ENABLED(FILESYSTEM_ASSETS)
	bool bOnFilesystem = Asset::DoesFileExist(id) && s_bUseFileAssets;
	if (!bInCooked && !bOnFilesystem)
	{
		// Asset doesn't exist
		LOG_E("[Repository] Asset not present in cooked archive or on filesystem! [%s]", id.data());
		if (onAssetMissing)
		{
			onAssetMissing();
		}
	}
	else
	{
		if (!bInCooked)
		{
			// Not in cooked archive (but on filesystem)
			LOG_W("[Repository] Asset present on filesystem but not in cooked archive! [%s]", id.data());
		}
		if (!bOnFilesystem)
		{
			// Not on filesystem (but in cooked archive)
			LOGIF_W(s_bUseFileAssets, "[Repository] Asset present in cooked archive but not on filesystem! [%s]", id.data());
			inCooked();
		}
		else
		{
			onFilesystem();
		}
	}
#else
	if (!bInCooked)
	{
		LOG_E("[Repository] Asset not present in cooked archive! [%s]", id.data());
		if (onAssetMissing)
		{
			onAssetMissing();
		}
	}
	else
	{
		inCooked();
	}
#endif
}
} // namespace LittleEngine