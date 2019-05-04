#pragma once
#include <mutex>
#include <future>
#include "CoreTypes.h"
#include "Logger.h"

namespace Core
{
class ArchiveReader;
}

namespace LittleEngine
{
// \brief Class that handles all Asset Loading. (Maintains a clearable cache)
class EngineRepository final
{
private:
	using Lock = std::lock_guard<std::mutex>;

private:
	UPtr<Core::ArchiveReader> m_uCooked;
	List<UPtr<class ManifestLoader>> m_uAsyncLoaders;
	std::mutex m_mutex;
	UMap<String, Asset::Ptr> m_loaded;
	String m_rootDir;
	class FontAsset* m_pDefaultFont;

public:
	EngineRepository(String archivePath, String rootDir = "");
	~EngineRepository();

	// Loads Asset at path. T must derive from Asset!
	template <typename T>
	T* Load(String id);
	template <typename T>
	std::future<T*> LoadAsync(String id);

#if ENABLED(FILESYSTEM_ASSETS)
	ManifestLoader* LoadAsync(String manifestPath, std::function<void()> onComplete = nullptr);
#endif
	ManifestLoader* LoadAsync(String archivePath,
							  String manifestPath,
							  std::function<void()> onComplete = nullptr);

	FontAsset* GetDefaultFont() const;

	bool Unload(String id);
	// Unload all assets
	void UnloadAll(bool bUnloadDefaultFont);

public:
	EngineRepository(const EngineRepository&) = delete;
	EngineRepository& operator=(const EngineRepository&) = delete;

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
	UPtr<T> FetchAsset(const String& id);
#endif

	bool IsLoaded(const String& id);

	void Tick(Time dt);

	friend class EngineService;
	friend class ManifestLoader;
};

template <typename T>
T* EngineRepository::Load(String id)
{
	static_assert(IsDerived<Asset, T>(),
				  "T must derive from Asset: check Output window for erroneous call");

	T* pT = GetLoaded<T>(id);
	if (pT)
	{
		return pT;
	}

	LOG_W("[EngineRepository] Orphaned asset (not loaded by manifest) requested at runtime [%s]", id.c_str());
	if (m_uCooked->IsPresent(id.c_str()))
	{
		pT = LoadFromArchive<T>(id);
	}
	if (!pT)
	{
#if ENABLED(FILESYSTEM_ASSETS)
		LOG_W("[EngineRepository] Asset not present in cooked archive [%s]", id.c_str());
		pT = LoadFromFilesystem<T>(id);
#else
		LOG_E("[EngineRepository] Asset not present in cooked archive [%s]", id.c_str());
#endif
	}
	return pT;
}

template <typename T>
std::future<T*> EngineRepository::LoadAsync(String id)
{
	static_assert(IsDerived<Asset, T>(),
				  "T must derive from Asset: check Output window for erroneous call");
	// std::function needs to be copyable, so cannot use UPtr<promise> here
	SPtr<std::promise<T*>> sPromise = MakeShared<std::promise<T*>>();

	T* pT = GetLoaded<T>(id);
	if (pT)
	{
		sPromise->set_value(pT);
		return sPromise->get_future();
	}

	if (m_uCooked->IsPresent(id.c_str()))
	{
		Services::Jobs()->Enqueue(
			[this, id, sPromise]() { sPromise->set_value(LoadFromArchive<T>(id)); }, "", true);
	}
	else
	{
#if ENABLED(FILESYSTEM_ASSETS)
		LOG_W("[EngineRepository] Asset not present in cooked archive [%s]", id.c_str());
		Services::Jobs()->Enqueue(
			[this, id, sPromise]() { sPromise->set_value(LoadFromFilesystem<T>(id)); }, "", true);
#else
		LOG_E("[EngineRepository] Asset not present in cooked archive [%s]", id.c_str());
		sPromise->set_value(nullptr);
#endif
	}

	return sPromise->get_future();
}

template <typename T>
T* EngineRepository::GetLoaded(const String& id)
{
	Lock lock(m_mutex);
	auto search = m_loaded.find(id);
	if (search != m_loaded.end())
	{
#if LOG_CACHED_ASSET_LOADS
		LOG_D("[EngineRepository] Found Asset [%s] in cache", id.c_str());
#endif
		return dynamic_cast<T*>(search->second.get());
	}
	return nullptr;
}

template <typename T>
T* EngineRepository::LoadFromArchive(const String& id)
{
	T* pT = nullptr;
	UPtr<T> uT = CreateAsset<T>(id, m_uCooked->Decompress(id.c_str()));
	if (uT)
	{
		pT = uT.get();
		Lock lock(m_mutex);
		m_loaded.emplace(id, std::move(uT));
	}
	return pT;
}

#if ENABLED(FILESYSTEM_ASSETS)
template <typename T>
T* EngineRepository::LoadFromFilesystem(const String& id)
{
	T* pT = nullptr;
	UPtr<T> uT = FetchAsset<T>(id);
	if (uT)
	{
		pT = uT.get();
		Lock lock(m_mutex);
		m_loaded.emplace(id, std::move(uT));
	}
	return pT;
}
#endif

template <typename T>
UPtr<T> EngineRepository::CreateAsset(const String& id, Vec<u8> buffer)
{
	struct enable_smart : public T
	{
		enable_smart(String id, Vec<u8> buffer) : T(std::move(id), std::move(buffer))
		{
		}
	};

	UPtr<enable_smart> uT;
	uT = MakeUnique<enable_smart>(id, std::move(buffer));
	if (!uT || uT->IsError())
	{
		return nullptr;
	}
	LOG_I("[EngineRepository] Decompressed %s [%s]", g_szAssetType[ToIdx(uT->GetType())], id.c_str());
	return std::move(uT);
}

#if ENABLED(FILESYSTEM_ASSETS)
template <typename T>
UPtr<T> EngineRepository::FetchAsset(const String& id)
{
	struct enable_smart : public T
	{
		enable_smart(String id, const String& pathPrefix) : T(std::move(id), pathPrefix)
		{
		}
	};

	UPtr<enable_smart> uT;
	uT = MakeUnique<enable_smart>(id, m_rootDir);
	if (!uT || uT->IsError())
	{
		return nullptr;
	}
	LOG_I("[EngineRepository] Loaded %s from filesystem [%s]", g_szAssetType[ToIdx(uT->GetType())], id.c_str());
	return std::move(uT);
}
#endif
} // namespace LittleEngine
