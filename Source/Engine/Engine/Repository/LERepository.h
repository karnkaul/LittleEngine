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
extern FontAsset* g_pDefaultFont;

// \brief Class that handles all Asset Loading. (Maintains a clearable cache)
class LERepository final : private NoCopy
{
#if ENABLED(FILESYSTEM_ASSETS)
public:
	// Set false to force load assets from archive even when FILESYSTEM_ASSETS is enabled
	static bool s_bUseFileAssets;
#endif

public:
	enum class Search : u8
	{
		Cooked,
		Filesystem,
	};

	enum class State : u8
	{
		Idle,
		Active
	};

private:
	UPtr<Core::ArchiveReader> m_uCooked;
	Vec<SPtr<class ManifestLoader>> m_loaders;
	mutable std::mutex m_loadedMutex;
	UMap<VString, UPtr<Asset>> m_loaded;
	String m_rootDir;
	State m_state;

public:
	LERepository(String defaultFontID, String archivePath, String rootDir = "");
	~LERepository();

	void LoadDefaultFont(String id);

	template <typename T>
	T* Load(String id, bool bReload = false);
	template <typename T>
	T* Preload(String id);
	template <typename T>
	Deferred<T*> LoadAsync(String id);

	SPtr<ManifestLoader> LoadManifest(String manifestPath, Task onComplete = nullptr);
	void UnloadManifest(String manifestPath, Task onComplete = nullptr);

	bool IsBusy() const;
	bool IsLoaded(VString id) const;
	u64 LoadedBytes() const;
	bool IsPresent(VString id) const;

	bool Unload(VString id);
	void UnloadAll(bool bUnloadDefaultFont);

	void ResetState();
	void Tick(Time dt);

	
private:
	template <typename T>
	T* GetLoaded(VString id);
	template <typename T>
	T* LoadFromArchive(String id);
#if ENABLED(FILESYSTEM_ASSETS)
	template <typename T>
	T* LoadFromFilesystem(String id);
#endif
	template <typename T, typename... U>
	UPtr<T> CreateAsset(U... args);
	template <typename T>
	UPtr<T> ConjureAsset(VString id, InitList<Search> searchOrder);

	void DoLoad(VString id, bool bSyncWarn, Task inCooked, 
#if ENABLED(FILESYSTEM_ASSETS)
		Task onFilesystem = nullptr, 
#endif
		Task onAssetMissing = nullptr);

	friend class ManifestLoader;
	friend class ILoadingHUD;
};

template <typename T>
T* LERepository::Load(String id, bool bReload)
{
	static_assert(IsDerived<Asset, T>(), "T must derive from Asset!");
	if (bReload)
	{
		Unload(id);
	}
	T* pT = GetLoaded<T>(id);
	if (!pT)
	{
		DoLoad(
			id, !bReload, [&]() { pT = LoadFromArchive<T>(std::move(id)); },
#if ENABLED(FILESYSTEM_ASSETS)
			[&]() { pT = LoadFromFilesystem<T>(std::move(id)); },
#endif
			nullptr
		);
	}
	return pT;
}

template <typename T>
T* LERepository::Preload(String id)
{
	static_assert(IsDerived<Asset, T>(), "T must derive from Asset!");
	Assert(m_state != State::Active, "Preloading when active!");
	if (m_state == State::Active)
	{
		LOG_E("[Repository] Cannot Preload once Repository is active!");
		return nullptr;
	}
	T* pT = GetLoaded<T>(id);
	if (!pT)
	{
		DoLoad(
			id, false, [&]() { pT = LoadFromArchive<T>(std::move(id)); },
#if ENABLED(FILESYSTEM_ASSETS)
			[&]() { pT = LoadFromFilesystem<T>(std::move(id)); },
#endif
			nullptr
		);
	}
	return pT;
}

template <typename T>
Deferred<T*> LERepository::LoadAsync(String id)
{
	static_assert(IsDerived<Asset, T>(), "T must derive from Asset!");
	// std::function needs to be copyable, so cannot use UPtr<promise> here
	SPtr<std::promise<T*>> sPromise = MakeShared<std::promise<T*>>();
	Deferred<T*> deferred = sPromise->get_future();
	T* pT = GetLoaded<T>(id);
	if (pT)
	{
		sPromise->set_value(pT);
	}
	else
	{
		DoLoad(
			id, false,
			[&]() { Core::Jobs::Enqueue([&, sPromise, id]() { sPromise->set_value(LoadFromArchive<T>(std::move(id))); }, "", true); },
#if ENABLED(FILESYSTEM_ASSETS)
			[&]() { Core::Jobs::Enqueue([&, sPromise, id]() { sPromise->set_value(LoadFromFilesystem<T>(std::move(id))); }, "", true); },
#endif
			[&]() { sPromise->set_value(nullptr); });
	}
	return deferred;
}

template <typename T>
T* LERepository::GetLoaded(VString id)
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
T* LERepository::LoadFromArchive(String id)
{
	T* pT = nullptr;
	auto buffer = m_uCooked->Decompress(id.c_str());
	UPtr<T> uT = CreateAsset<T>(std::move(id), std::move(buffer));
	if (uT)
	{
		pT = uT.get();
		Lock lock(m_loadedMutex);
		m_loaded[uT->ID()] = std::move(uT);
	}
	return pT;
}

#if ENABLED(FILESYSTEM_ASSETS)
template <typename T>
T* LERepository::LoadFromFilesystem(String id)
{
	T* pT = nullptr;
	UPtr<T> uT = CreateAsset<T>(std::move(id));
	if (uT)
	{
		pT = uT.get();
		Lock lock(m_loadedMutex);
		m_loaded[uT->ID()] = std::move(uT);
	}
	return pT;
}
#endif

template <typename T, typename... U>
UPtr<T> LERepository::CreateAsset(U... args)
{
	UPtr<T> uT;
	uT = MakeUnique<T>(std::forward<U>(args)...);
	if (!uT || uT->IsError())
	{
		return nullptr;
	}
	auto size = Core::FriendlySize(uT->ByteCount());
	LOG_I("== [%s] [%.2f%s] %s created", uT->ID().data(), size.first, size.second.data(), g_szAssetType[ToIdx(uT->Type())].data());
	return (uT);
}

template <typename T>
UPtr<T> LERepository::ConjureAsset(VString id, InitList<Search> searchOrder)
{
	UPtr<T> uT;
#if ENABLED(FILESYSTEM_ASSETS)
	auto retrieveAsset = [&]() {
		if (!Asset::DoesFileExist(id))
		{
			LOG_E("[Repository] Asset %s not present on filesystem!", id.data());
		}
		else
		{
			uT = CreateAsset<T>(String(id));
			if (!uT || uT->IsError())
			{
				LOG_E("[Repository] Could not load %s from filesystem assets!", id.data());
			}
		}
	};
#endif
	auto decompressAsset = [&]() {
		if (!m_uCooked->IsPresent(id))
		{
			LOG_E("[Repository] Asset %s not present in cooked archive!", id.data());
		}
		else
		{
			uT = CreateAsset<T>(String(id), m_uCooked->Decompress(id));
			if (!uT || uT->IsError())
			{
				LOG_E("[Repository] Could not load %s from cooked assets!", id.data());
			}
		}
	};
	for (auto search : searchOrder)
	{
		switch (search)
		{
		default:
		case Search::Cooked:
			decompressAsset();
			break;
		case Search::Filesystem:
#if ENABLED(FILESYSTEM_ASSETS)
			if (s_bUseFileAssets)
			{
				retrieveAsset();
			}
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
