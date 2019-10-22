#pragma once
#include "Core/Game/ArchiveReader.h"
#include "Core/CoreTypes.h"
#include "Core/Game/Jobs.h"
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
	std::unique_ptr<Core::ArchiveReader> m_uCooked;
	std::vector<std::shared_ptr<class ManifestLoader>> m_loaders;
	mutable std::mutex m_loadedMutex;
	std::unordered_map<std::string, std::unique_ptr<Asset>> m_loaded;
	std::string m_rootDir;
	State m_state = State::Idle;

public:
	LERepository(std::string defaultFontID, std::string archivePath, std::string rootDir = "");
	~LERepository();

	void LoadDefaultFont(std::string id);

	template <typename T>
	T* Load(std::string id, bool bReload = false);
	template <typename T>
	T* Preload(std::string id);
	template <typename T>
	Deferred<T*> LoadAsync(std::string id);

	std::shared_ptr<ManifestLoader> LoadManifest(std::string manifestPath, Task onComplete = nullptr);
	void UnloadManifest(std::string manifestPath, Task onComplete = nullptr);

	bool IsBusy() const;
	bool IsLoaded(const std::string& id) const;
	u64 LoadedBytes() const;
	bool IsPresent(const std::string& id) const;

	bool Unload(const std::string& id);
	void UnloadAll(bool bUnloadDefaultFont);

	void ResetState();
	void Tick(Time dt);

	
private:
	template <typename T>
	T* GetLoaded(const std::string& id);
	template <typename T>
	T* LoadFromArchive(std::string id);
#if ENABLED(FILESYSTEM_ASSETS)
	template <typename T>
	T* LoadFromFilesystem(std::string id);
#endif
	template <typename T, typename... U>
	std::unique_ptr<T> CreateAsset(U... args);
	template <typename T>
	std::unique_ptr<T> ConjureAsset(const std::string& id, std::initializer_list<Search> searchOrder);

	void DoLoad(const std::string& id, bool bSyncWarn, Task inCooked, 
#if ENABLED(FILESYSTEM_ASSETS)
		Task onFilesystem = nullptr, 
#endif
		Task onAssetMissing = nullptr);

	friend class ManifestLoader;
	friend class ILoadingHUD;
};

template <typename T>
T* LERepository::Load(std::string id, bool bReload)
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
T* LERepository::Preload(std::string id)
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
Deferred<T*> LERepository::LoadAsync(std::string id)
{
	static_assert(IsDerived<Asset, T>(), "T must derive from Asset!");
	// std::function needs to be copyable, so cannot use std::unique_ptr<promise> here
	std::shared_ptr<std::promise<T*>> sPromise = std::make_shared<std::promise<T*>>();
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
T* LERepository::GetLoaded(const std::string& id)
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
T* LERepository::LoadFromArchive(std::string id)
{
	T* pT = nullptr;
	auto buffer = m_uCooked->Decompress(id.c_str());
	auto uT = CreateAsset<T>(std::move(id), std::move(buffer));
	if (uT)
	{
		pT = uT.get();
		Lock lock(m_loadedMutex);
		m_loaded[std::string(uT->ID())] = std::move(uT);
	}
	return pT;
}

#if ENABLED(FILESYSTEM_ASSETS)
template <typename T>
T* LERepository::LoadFromFilesystem(std::string id)
{
	T* pT = nullptr;
	auto uT = CreateAsset<T>(std::move(id));
	if (uT)
	{
		pT = uT.get();
		Lock lock(m_loadedMutex);
		m_loaded[std::string(uT->ID())] = std::move(uT);
	}
	return pT;
}
#endif

template <typename T, typename... U>
std::unique_ptr<T> LERepository::CreateAsset(U... args)
{
	std::unique_ptr<T> uT;
	uT = std::make_unique<T>(std::forward<U>(args)...);
	if (!uT || uT->IsError())
	{
		return nullptr;
	}
	auto size = Core::FriendlySize(uT->ByteCount());
	LOG_I("== [%s] [%.2f%s] %s created", uT->ID().data(), size.first, size.second.data(), g_szAssetType[ToIdx(uT->Type())].data());
	return (uT);
}

template <typename T>
std::unique_ptr<T> LERepository::ConjureAsset(const std::string& id, std::initializer_list<Search> searchOrder)
{
	std::unique_ptr<T> uT;
#if ENABLED(FILESYSTEM_ASSETS)
	auto retrieveAsset = [&]() {
		if (!Asset::DoesFileExist(id))
		{
			LOG_E("[Repository] Asset %s not present on filesystem!", id.data());
		}
		else
		{
			uT = CreateAsset<T>(id);
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
			uT = CreateAsset<T>(id, m_uCooked->Decompress(id));
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
