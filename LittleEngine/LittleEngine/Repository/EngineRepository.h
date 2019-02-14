#pragma once
#include <mutex>
#include "CoreTypes.h"
#include "SFMLAPI/System/SFAssets.h"

namespace LittleEngine
{
// \brief Class that handles all Asset Loading. (Maintains a clearable cache)
class EngineRepository final
{
private:
	using Lock = std::lock_guard<std::mutex>;

	List<UPtr<class AsyncAssetLoader>> m_uAsyncLoaders;
	std::mutex m_mutex;
	UMap<String, Asset::Ptr> m_loaded;
	String m_rootDir;
	FontAsset* m_pDefaultFont;

public:
	EngineRepository(const String& rootDir = "");
	~EngineRepository();

	// Loads Asset at path. T must derive from Asset!
	// Note: Not meant to be used in hot code!
	template <typename T>
	T* Load(String path)
	{
		static_assert(IsDerived(Asset, T),
					  "T must derive from Asset: check Output window for erroneous call");
		String fullPath = GetPath(path);
		m_mutex.lock();
		auto search = m_loaded.find(fullPath);
		m_mutex.unlock();

		if (search != m_loaded.end())
		{
#if LOG_CACHED_ASSET_LOADS
			LogD("[EngineRepository] Found Asset [" + fullPath + "] in cache");
#endif
			return dynamic_cast<T*>(search->second.get());
		}

		UPtr<T> uT = LoadInternal<T>(fullPath);
		T* pT = uT.get();

		m_mutex.lock();
		m_loaded.emplace(fullPath, std::move(uT));
		m_mutex.unlock();
		return pT;
	}

	// Note: Not meant to be used in hot code!
	template <typename T>
	Vector<T*> Load(InitList<String> assetPaths)
	{
		static_assert(IsDerived(Asset, T),
					  "T must derive from Asset: check Output window for erroneous call");
		Vector<T*> vec;
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
	Vector<T*> Load(Vector<String> assetPaths)
	{
		static_assert(IsDerived(Asset, T),
					  "T must derive from Asset: check Output window for erroneous call");
		Vector<T*> vec;
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
	void LoadAsync(const String& manifestPath, Function(void()) OnComplete = nullptr);

	FontAsset* GetDefaultFont() const;

	// Unload all assets
	void UnloadAll();

private:
	EngineRepository(const EngineRepository&) = delete;
	EngineRepository& operator=(const EngineRepository&) = delete;

	String GetPath(const String& path) const;

	template <typename T>
	UPtr<T> LoadInternal(const String& fullPath)
	{
		LogI("[AssetManager] Loading Asset [" + fullPath + "]");
		struct enable_smart : public T
		{
			enable_smart(const String& path) : T(path)
			{
			}
		};
		return MakeUnique<enable_smart>(fullPath);
	}

	bool IsLoaded(const String& fullPath);

	void Tick(Time dt);

	friend class EngineService;
	friend class AsyncAssetLoader;
};
} // namespace LittleEngine
