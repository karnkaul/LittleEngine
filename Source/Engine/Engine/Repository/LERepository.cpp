#include <fstream>
#include <optional>
#include "Core/OS.h"
#include "LERepository.h"
#include "AssetManifest.h"
#include "ManifestLoader.h"
#include "Engine/FatalEngineException.h"

namespace LittleEngine
{
namespace
{
std::optional<String> FindFile(const String& name, Vec<String> paths)
{
	std::optional<String> ret;
	for (const auto& path : paths)
	{
		if (std::ifstream(path + "/" + name).good())
		{
			ret = path;
			break;
		}
	}
	return ret;
}
} // namespace

LERepository* g_pRepository = nullptr;
#if ENABLED(FILESYSTEM_ASSETS)
bool LERepository::s_bUseFileAssets = true;
#endif

LERepository::LERepository(String defaultFontID, String archivePath, String rootDir) : m_rootDir(std::move(rootDir))
{
	OS::EnvData* pEnv = OS::Env();
	auto prefix = FindFile(archivePath, {".", pEnv->ExePath(), pEnv->RuntimePath()});
	if (prefix)
	{
		archivePath = *prefix + "/" + std::move(archivePath);
		m_rootDir = std::move(*prefix) + "/" + std::move(m_rootDir);
	}

	m_assetPathPrefix = m_rootDir.empty() ? String() : m_rootDir + "/";
	m_uCooked = MakeUnique<Core::ArchiveReader>();

	LOG_D("[Repository] Assets Root Dir: %s", m_rootDir.c_str());

	std::ifstream file(archivePath.c_str());
	Assert(file.good(), "Cooked archive does not exist!");
#if defined(DEBUGGING)
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
		LOG_E("[Repository] Cooked archive [%s] does not exist!", archivePath.c_str());
#if !ENABLED(FILESYSTEM_ASSETS)
		throw FatalEngineException();
#endif
	}
	else
	{
		LOG_D("[Repository] Located cooked archive at [%s]", archivePath.c_str());
		m_uCooked->Load(archivePath.c_str());
	}

	LoadDefaultFont(std::move(defaultFontID));
	g_pRepository = this;
	LOG_D("[Repository] constructed");
}

LERepository::~LERepository()
{
	g_pRepository = nullptr;
	m_pDefaultFont = nullptr;
	m_loaded.clear();
	LOG_D("[Repository] destroyed");
}

void LERepository::LoadDefaultFont(String id) 
{
	if (m_loaded.find(id) == m_loaded.end())
	{
		m_pDefaultFont = nullptr;
		auto uFont = ConjureAsset<FontAsset>(id, false, {Search::Filesystem, Search::Cooked});
		if (uFont)
		{
			m_pDefaultFont = uFont.get();
			m_loaded.emplace(std::move(id), std::move(uFont));
		}
	}
	Assert(m_pDefaultFont, "Invariant violated: Default Font is null!");
}

FontAsset* LERepository::DefaultFont() const
{
	Assert(m_pDefaultFont, "Default Font has not been set!");
	return m_pDefaultFont;
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
	bRet |= std::ifstream(FileAssetPath(id)).good();
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
	if (bUnloadDefaultFont || !m_pDefaultFont)
	{
		m_loaded.clear();
	}
	else
	{
		String fontID = m_pDefaultFont->ID();
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

#if ENABLED(FILESYSTEM_ASSETS)
bool LERepository::DoesFileAssetExist(const String& id)
{
	std::ifstream file(FileAssetPath(id).c_str());
	return file.good();
}
String LERepository::FileAssetPath(const String& id) const
{
	return m_assetPathPrefix + id;
}
#endif
} // namespace LittleEngine
