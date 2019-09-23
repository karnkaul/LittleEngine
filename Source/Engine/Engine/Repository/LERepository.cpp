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

	String filePath = OS::Env()->FullPath(archivePath.c_str());
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
		auto uFont = ConjureAsset<FontAsset>(id, false, {Search::Filesystem, Search::Cooked});
		if (uFont)
		{
			g_pDefaultFont = uFont.get();
			m_loaded.emplace(std::move(id), std::move(uFont));
		}
	}
	Assert(g_pDefaultFont, "Invariant violated: Default Font is null!");
}

ManifestLoader* LERepository::LoadManifest(String manifestPath, Task onComplete)
{
	UPtr<ManifestLoader> uLoader = MakeUnique<ManifestLoader>(*this, std::move(manifestPath), std::move(onComplete), false);
	ManifestLoader* pLoader = uLoader.get();
	m_loaders.emplace_back(std::move(uLoader));
	return pLoader;
}

ManifestLoader* LERepository::UnloadManifest(String manifestPath, Task onComplete /* = nullptr */)
{
	UPtr<ManifestLoader> uLoader = MakeUnique<ManifestLoader>(*this, std::move(manifestPath), std::move(onComplete), true);
	ManifestLoader* pLoader = uLoader.get();
	m_loaders.emplace_back(std::move(uLoader));
	return pLoader;
}

bool LERepository::IsLoaded(const String& id) const
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

bool LERepository::IsPresent(const String& id) const
{
	bool bRet = m_uCooked->IsPresent(id.c_str());
#if ENABLED(FILESYSTEM_ASSETS)
	bRet |= Asset::DoesFileExist(id);
#endif
	return bRet;
}

bool LERepository::Unload(String id)
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
		String fontID = g_pDefaultFont->ID();
		Core::RemoveIf<String, UPtr<Asset>>(m_loaded, [fontID](UPtr<Asset>& uAsset) { return uAsset->ID() != fontID; });
	}
	LOG_D("[Repository] cleared");
}

bool LERepository::IsBusy() const
{
	bool bLoading = false;
	for (auto& uLoader : m_loaders)
	{
		bLoading |= !uLoader->m_bUnloading;
	}
	return bLoading;
}

void LERepository::ResetState()
{
	m_state = State::Idle;
}

void LERepository::Tick(Time dt)
{
	m_state = State::Active;
	auto iter = m_loaders.begin();
	while (iter != m_loaders.end())
	{
		(*iter)->Tick(dt);
		if ((*iter)->m_bIdle)
		{
			iter = m_loaders.erase(iter);
			continue;
		}
		++iter;
	}
}
} // namespace LittleEngine
