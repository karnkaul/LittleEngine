#pragma once
#include "Core/ArchiveReader.h"
#include "Core/CoreTypes.h"
#include "Core/Logger.h"
#include "Core/Jobs.h"
#include "Core/Utils.h"
#include "SFMLAPI/System/Assets.h"

namespace Core
{
class ArchiveReader;
}

namespace LittleEngine
{
extern class LERepository* g_pRepository;

// \brief Class that handles all Asset Loading. (Maintains a clearable cache)
class LERepository final
{
#if ENABLED(FILESYSTEM_ASSETS)
public:
	// Set false to force load assets from archive even when FILESYSTEM_ASSETS is enabled
	static bool s_bUseFileAssets;
#endif

public:
	enum Scope
	{
		COOKED,
		FILESYSTEM,
	};

	enum class State
	{
		Idle,
		Active
	};

private:
	UPtr<Core::ArchiveReader> m_uCooked;
	List<UPtr<class ManifestLoader>> m_loaders;
	mutable std::mutex m_loadedMutex;
	UMap<String, Asset::Ptr> m_loaded;
	String m_rootDir;
	String m_assetPathPrefix;
	State m_state;
	class FontAsset* m_pDefaultFont = nullptr;

public:
	LERepository(String defaultFontID, String archivePath, String rootDir = "");
	~LERepository();

	// Loads Asset at path. T must derive from Asset!
	template <typename T>
	T* Load(String id, bool bReload = false);
	template <typename T>
	Deferred<T*> LoadAsync(String id);

	ManifestLoader* LoadManifest(String manifestPath, Task onComplete = nullptr);
	ManifestLoader* UnloadManifest(String manifestPath, Task onComplete = nullptr);

	FontAsset* DefaultFont() const;

	bool IsLoaded(const String& id) const;
	u64 LoadedBytes() const;
	bool IsPresent(const String& id) const;

	bool Unload(String id);
	// Unload all assets
	void UnloadAll(bool bUnloadDefaultFont);

	bool IsBusy() const;

public:
	LERepository(const LERepository&) = delete;
	LERepository& operator=(const LERepository&) = delete;

	void Tick(Time dt);

	template <typename T>
	T* Preload(const String& id);

private:
	template <typename T>
	T* GetLoaded(const String& id);

	template <typename T>
	T* LoadFromArchive(const String& id);
#if ENABLED(FILESYSTEM_ASSETS)
	template <typename T>
	T* LoadFromFilesystem(const String& id);
#endif

	template <typename T>
	UPtr<T> CreateAsset(const String& id, Vec<u8> buffer);
#if ENABLED(FILESYSTEM_ASSETS)
	template <typename T>
	UPtr<T> RetrieveAsset(const String& id);
#endif

	template <typename T>
	UPtr<T> ConjureAsset(const String& id, bool bSilent, InitList<Scope> searchOrder);

#if ENABLED(FILESYSTEM_ASSETS)
	bool DoesFileAssetExist(const String& id);
	String FileAssetPath(const String& id) const;
#endif

	friend class ManifestLoader;
	friend class ILoadingHUD;
};

template <typename T>
T* LERepository::Load(String id, bool bReload)
{
	static_assert(IsDerived<Asset, T>(), "T must derive from Asset: check Output window for erroneous call");

	if (bReload)
	{
		Unload(id);
	}

	T* pT = GetLoaded<T>(id);
	if (pT)
	{
		return pT;
	}

	if (!bReload)
	{
		LOG_W("[Repository] Synchronously loading Asset (add id to manifest or use LoadAsync() "
			  "to "
			  "suppress warning) [%s]",
			  id.c_str());
	}

	bool bInCooked = m_uCooked->IsPresent(id.c_str());
#if ENABLED(FILESYSTEM_ASSETS)
	bool bOnFilesystem = DoesFileAssetExist(id) && s_bUseFileAssets;
	// Asset doesn't exist
	if (!bInCooked && !bOnFilesystem)
	{
		if (!bReload)
		{
			LOG_E("[Repository] Asset not present in cooked archive or on filesystem! [%s]", id.c_str());
		}
		return nullptr;
	}
	// Not in cooked archive (but on filesystem)
	if (!bInCooked)
	{
		if (!bReload)
		{
			LOG_W("[Repository] Asset present on filesystem but not in cooked archive! [%s]", id.c_str());
		}
	}
	// Not on filesystem (but in cooked archive)
	if (!bOnFilesystem)
	{
		if (!bReload)
		{
			LOG_W("[Repository] Asset present in cooked archive but not on filesystem! [%s]", id.c_str());
		}
		pT = LoadFromArchive<T>(id);
	}
	// On filesystem: load that regardless of cooked asset
	else
	{
		pT = LoadFromFilesystem<T>(id);
	}
#else
	if (!bInCooked)
	{
		LOG_E("[Repository] Asset not present in cooked archive! [%s]", id.c_str());
	}
	else
	{
		pT = LoadFromArchive<T>(id);
	}
#endif
	return pT;
}

template <typename T>
Deferred<T*> LERepository::LoadAsync(String id)
{
	static_assert(IsDerived<Asset, T>(), "T must derive from Asset: check Output window for erroneous call");
	// std::function needs to be copyable, so cannot use UPtr<promise> here
	SPtr<std::promise<T*>> sPromise = MakeShared<std::promise<T*>>();
	Deferred<T*> deferred = sPromise->get_future();

	T* pT = GetLoaded<T>(id);
	if (pT)
	{
		sPromise->set_value(pT);
		return deferred;
	}

	bool bInCooked = m_uCooked->IsPresent(id.c_str());
#if ENABLED(FILESYSTEM_ASSETS)
	bool bOnFilesystem = DoesFileAssetExist(id) && s_bUseFileAssets;
	if (!bInCooked && !bOnFilesystem)
	{
		LOG_E("[Repository] Asset not present in cooked archive or on filesystem! [%s]", id.c_str());
		sPromise->set_value(nullptr);
		return deferred;
	}
	// Not in cooked archive (but on filesystem)
	if (!bInCooked)
	{
		LOG_W("[Repository] Asset present on filesystem but not in cooked archive! [%s]", id.c_str());
	}
	// Not on filesystem (but in cooked archive)
	if (!bOnFilesystem)
	{
		LOG_W("[Repository] Asset present in cooked archive but not on filesystem! [%s]", id.c_str());
		Core::Jobs::Enqueue([&, sPromise, id]() { sPromise->set_value(LoadFromArchive<T>(std::move(id))); }, "", true);
	}
	// On filesystem: load that regardless of cooked asset
	else
	{
		Core::Jobs::Enqueue([&, sPromise, id]() { sPromise->set_value(LoadFromFilesystem<T>(std::move(id))); }, "", true);
	}
#else
	if (!bInCooked)
	{
		LOG_E("[Repository] Asset not present in cooked archive! [%s]", id.c_str());
		sPromise->set_value(nullptr);
	}
	else
	{
		Core::Jobs::Enqueue([&, sPromise, id]() { sPromise->set_value(LoadFromArchive<T>(std::move(id))); }, "", true);
	}
#endif
	return deferred;
}

template <typename T>
T* LERepository::Preload(const String& id)
{
	Assert(m_state != State::Active, "Preloading when active!");
	if (m_state == State::Active)
	{
		LOG_E("[Repository] Cannot Preload once Repository is active!");
		return nullptr;
	}
	T* pT = GetLoaded<T>(id);
	if (pT)
	{
		return pT;
	}
	bool bInCooked = m_uCooked->IsPresent(id.c_str());
#if ENABLED(FILESYSTEM_ASSETS)
	bool bOnFilesystem = DoesFileAssetExist(id) && s_bUseFileAssets;
	// Asset doesn't exist
	if (!bInCooked && !bOnFilesystem)
	{
		LOG_E("[Repository] (Preload) Asset not present in cooked archive or on filesystem! [%s]", id.c_str());
		return nullptr;
	}
	// Not in cooked archive (but on filesystem)
	if (!bInCooked)
	{
		LOG_W("[Repository] (Preload) Asset present on filesystem but not in cooked archive! [%s]", id.c_str());
	}
	// Not on filesystem (but in cooked archive)
	if (!bOnFilesystem)
	{
		LOG_W("[Repository] (Preload) Asset present in cooked archive but not on filesystem! [%s]", id.c_str());
		pT = LoadFromArchive<T>(id);
	}
	// On filesystem: load that regardless of cooked asset
	else
	{
		pT = LoadFromFilesystem<T>(id);
	}
#else
	if (!bInCooked)
	{
		LOG_E("[Repository] (Preload) Asset not present in cooked archive! [%s]", id.c_str());
	}
	else
	{
		pT = LoadFromArchive<T>(id);
	}
#endif
	return pT;
}

template <typename T>
T* LERepository::GetLoaded(const String& id)
{
	Lock lock(m_loadedMutex);
	auto search = m_loaded.find(id);
	if (search != m_loaded.end())
	{
		return dynamic_cast<T*>(search->second.get());
	}
	return nullptr;
}

template <typename T>
T* LERepository::LoadFromArchive(const String& id)
{
	T* pT = nullptr;
	UPtr<T> uT = CreateAsset<T>(id, m_uCooked->Decompress(id.c_str()));
	if (uT)
	{
		pT = uT.get();
		Lock lock(m_loadedMutex);
		m_loaded.emplace(id, std::move(uT));
	}
	return pT;
}

#if ENABLED(FILESYSTEM_ASSETS)
template <typename T>
T* LERepository::LoadFromFilesystem(const String& id)
{
	T* pT = nullptr;
	UPtr<T> uT = RetrieveAsset<T>(id);
	if (uT)
	{
		pT = uT.get();
		Lock lock(m_loadedMutex);
		m_loaded.emplace(id, std::move(uT));
	}
	return pT;
}
#endif

template <typename T>
UPtr<T> LERepository::CreateAsset(const String& id, Vec<u8> buffer)
{
	struct enable_smart : public T
	{
		enable_smart(String id, Vec<u8> buffer) : T(std::move(id), std::move(buffer)) {}
	};

	UPtr<enable_smart> uT;
	uT = MakeUnique<enable_smart>(id, std::move(buffer));
	if (!uT || uT->IsError())
	{
		return nullptr;
	}
	auto size = Core::FriendlySize(uT->ByteCount());
	LOG_I("== [%s] [%.2f%s] %s decompressed", id.c_str(), size.first, size.second, g_szAssetType[ToIdx(uT->Type())]);
	return (uT);
}

#if ENABLED(FILESYSTEM_ASSETS)
template <typename T>
UPtr<T> LERepository::RetrieveAsset(const String& id)
{
	struct enable_smart : public T
	{
		enable_smart(String id, const String& pathPrefix) : T(std::move(id), pathPrefix) {}
	};

	UPtr<enable_smart> uT;
	uT = MakeUnique<enable_smart>(id, m_rootDir);
	if (!uT || uT->IsError())
	{
		return nullptr;
	}
	auto size = Core::FriendlySize(uT->ByteCount());
	LOG_I("== [%s] [%.2f%s] %s loaded from filesystem", id.c_str(), size.first, size.second, g_szAssetType[ToIdx(uT->Type())]);
	return (uT);
}
#endif

template <typename T>
UPtr<T> LERepository::ConjureAsset(const String& id, bool bSilent, InitList<Scope> searchOrder)
{
	UPtr<T> uT;

#if ENABLED(FILESYSTEM_ASSETS)
	auto retrieveAsset = [&]() {
		if (!DoesFileAssetExist(id))
		{
			if (!bSilent)
			{
				LOG_E("[Repository] Asset %s not present on filesystem!", id.c_str());
			}
		}
		else
		{
			uT = RetrieveAsset<T>(id);
			if (!bSilent && (!uT || uT->IsError()))
			{
				LOG_E("[Repository] Could not load %s from filesystem assets!", id.c_str());
			}
		}
	};
#endif

	auto createAsset = [&]() {
		if (m_uCooked->IsPresent(id.c_str()))
		{
			uT = CreateAsset<T>(id, m_uCooked->Decompress(id.c_str()));
		}
		if (!bSilent && (!uT || uT->IsError()))
		{
			LOG_E("[Repository] Could not load %s from cooked assets!", id.c_str());
		}
	};

	for (auto search : searchOrder)
	{
		switch (search)
		{
		default:
		case Scope::COOKED:
			createAsset();
			break;
		case Scope::FILESYSTEM:
#if ENABLED(FILESYSTEM_ASSETS)
			retrieveAsset();
#endif
			break;
		}

		if (uT && !uT->IsError())
		{
			break;
		}
	}

	return (uT && !uT->IsError()) ? std::move(uT) : nullptr;
}
} // namespace LittleEngine
