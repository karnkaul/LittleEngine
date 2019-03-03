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
	// Note: Not meant to be used in hot code!
	template <typename T>
	T* Load(const String& id)
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

		T* pT = nullptr;
		
		if (!m_archiveReader.IsPresent(id.c_str()))
		{
			LOG_W("[EngineRepository] Cooked archive does not contain %s!", id.c_str());
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
				LOG_W("[EngineRepository] Could not load %s from cooked archive!", id.c_str());
			}
		}

#if DEBUGGING
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
				LOG_W("[EngineRepository] Could not load %s from filesystem!", id.c_str());
			}
		}
#endif
		return pT;
	}

	// Note: Not meant to be used in hot code!
	template <typename T>
	Vec<T*> Load(InitList<String> assetPaths)
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

	// Note: Not meant to be used in hot code!
	template <typename T>
	Vec<T*> Load(Vec<String> assetPaths)
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

	// Note: Not meant to be used in hot code!
	void LoadAll(AssetManifest& manifest);
	// Note: Will not load Music!
	AsyncAssetLoader* LoadAsync(const String& manifestPath, const std::function<void()>& onComplete = nullptr);
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
	UPtr<T> LoadInternal(const String& id)
	{
		LOG_I("[AssetManager] Loading Asset [%s]", id.c_str());
		struct enable_smart : public T
		{
			enable_smart(const String& id, const String& pathPrefix) : T(id, pathPrefix)
			{
			}
		};

		UPtr<enable_smart> uT; 
		try
		{
			uT = MakeUnique<enable_smart>(id, m_rootDir);
		}
		catch (const AssetLoadException& e)
		{
		}
		return std::move(uT);
	}

	template<typename T>
	UPtr<T> LoadInternal(const String& id, const Vec<u8>& buffer)
	{
		LOG_I("[AssetManager] Decompressing Asset [%s]", id.c_str());
		struct enable_smart : public T
		{
			enable_smart(const String& id, const Vec<u8>& buffer) : T(id, buffer)
			{
			}
		};

		UPtr<enable_smart> uT;
		try
		{
			uT = MakeUnique<enable_smart>(id, buffer);
		}
		catch (const AssetLoadException& e)
		{
		}
		return std::move(uT);
	}

	bool IsLoaded(const String& id);

	void Tick(Time dt);

	friend class EngineService;
	friend class AsyncAssetLoader;
};
} // namespace LittleEngine
