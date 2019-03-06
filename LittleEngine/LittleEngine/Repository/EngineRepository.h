#pragma once
#include <mutex>
#include "CoreTypes.h"
#include "ArchiveReader.h"
#include "SFMLAPI/System/SFAssets.h"

namespace LittleEngine
{
// \brief Class that handles all Asset Loading. (Maintains a clearable cache)
class EngineRepository final
{
private:
	using Lock = std::lock_guard<std::mutex>;

	Core::ArchiveReader m_archiveReader;
	List<UPtr<class AsyncAssetLoader>> m_uAsyncLoaders;
	std::mutex m_mutex;
	UMap<String, Asset::Ptr> m_loaded;
	String m_rootDir;
	FontAsset* m_pDefaultFont;

public:
	EngineRepository(const String& archivePath, const String& rootDir = "");
	~EngineRepository();

	// Loads Asset at path. T must derive from Asset!
	template <typename T>
	T* Load(const String& id);
	template <typename T>
	Vec<T*> Load(InitList<String> assetPaths);
	template <typename T>
	Vec<T*> Load(Vec<String> assetPaths);

	void LoadAll(AssetManifest& manifest);
#if !SHIPPING
	AsyncAssetLoader* LoadAsync(const String& manifestPath, const std::function<void()>& onComplete = nullptr);
#endif
	AsyncAssetLoader* LoadAsync(const String& archivePath,
								const String& manifestPath,
								const std::function<void()>& onComplete = nullptr);

	FontAsset* GetDefaultFont() const;

	// Unload all assets
	void UnloadAll(bool bUnloadDefaultFont);

private:
	EngineRepository(const EngineRepository&) = delete;
	EngineRepository& operator=(const EngineRepository&) = delete;

	template <typename T>
	UPtr<T> LoadInternal(const String& id, const Vec<u8>& buffer);

#if !SHIPPING
	template <typename T>
	UPtr<T> LoadInternal(const String& id);
#endif

	bool IsLoaded(const String& id);

	void Tick(Time dt);

	friend class EngineService;
	friend class AsyncAssetLoader;
};

template <typename T>
T* EngineRepository::Load(const String& id)
{
	static_assert(IsDerived<Asset, T>(),
				  "T must derive from Asset: check Output window for erroneous call");
	m_mutex.lock();
	auto search = m_loaded.find(id);
	m_mutex.unlock();

	if (search != m_loaded.end())
	{
#if LOG_CACHED_ASSET_LOADS
		LOG_D("[EngineRepository] Found Asset [%s] in cache", id.c_str());
#endif
		return dynamic_cast<T*>(search->second.get());
	}

	LOG_W("[EngineRepository] Orphaned asset (not loaded by manifest) requested at runtime [%s]", id.c_str());
	T* pT = nullptr;
	if (!m_archiveReader.IsPresent(id.c_str()))
	{
		LOG_E("[EngineRepository] Cooked archive does not contain [%s]!", id.c_str());
	}
	else
	{
		UPtr<T> uT = LoadInternal<T>(id, m_archiveReader.Decompress(id));
		if (uT)
		{
			pT = uT.get();
			m_mutex.lock();
			m_loaded.emplace(id, std::move(uT));
			m_mutex.unlock();
		}
		else
		{
			LOG_E("[EngineRepository] Could not load [%s] from cooked archive!", id.c_str());
		}
	}

#if !SHIPPING
	if (!pT)
	{
		UPtr<T> uT = LoadInternal<T>(id);
		if (uT)
		{
			pT = uT.get();
			m_mutex.lock();
			m_loaded.emplace(id, std::move(uT));
			m_mutex.unlock();
		}
		else
		{
			LOG_E("[EngineRepository] Could not load [%s] from filesystem!", id.c_str());
		}
	}
#endif
	return pT;
}

template <typename T>
Vec<T*> EngineRepository::Load(InitList<String> assetPaths)
{
	static_assert(IsDerived(Asset, T),
				  "T must derive from Asset: check Output window for erroneous call");
	Vec<T*> vec;
	for (const auto& path : assetPaths)
	{
		if (T* asset = Load<T>(path))
		{
			vec.emplace_back(asset);
		}
	}
	return vec;
}

template <typename T>
Vec<T*> EngineRepository::Load(Vec<String> assetPaths)
{
	static_assert(IsDerived<Asset, T>(),
				  "T must derive from Asset: check Output window for erroneous call");
	Vec<T*> vec;
	for (const auto& path : assetPaths)
	{
		if (T* asset = Load<T>(path))
		{
			vec.emplace_back(asset);
		}
	}
	return vec;
}

template <typename T>
UPtr<T> EngineRepository::LoadInternal(const String& id, const Vec<u8>& buffer)
{
	struct enable_smart : public T
	{
		enable_smart(const String& id, const Vec<u8>& buffer) : T(id, buffer)
		{
		}
	};

	UPtr<enable_smart> uT;
	uT = MakeUnique<enable_smart>(id, buffer);
	if (!uT || uT->IsError())
		return nullptr;
	LOG_I("[AssetManager] Decompressed %s [%s]", g_szAssetType[ToIdx(uT->GetType())], id.c_str());
	return std::move(uT);
}

#if !SHIPPING
template <typename T>
UPtr<T> EngineRepository::LoadInternal(const String& id)
{
	struct enable_smart : public T
	{
		enable_smart(const String& id, const String& pathPrefix) : T(id, pathPrefix)
		{
		}
	};

	UPtr<enable_smart> uT;
	uT = MakeUnique<enable_smart>(id, m_rootDir);
	if (!uT || uT->IsError())
		return nullptr;
	LOG_I("[AssetManager] Loaded %s from filesystem [%s]", g_szAssetType[ToIdx(uT->GetType())], id.c_str());
	return std::move(uT);
}
#endif
} // namespace LittleEngine
