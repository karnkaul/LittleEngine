#pragma once
#include <string>
#include <unordered_map>
#include "Engine/Object.h"
#include "Engine/Logger/Logger.h"
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "Utils.h"
#include "GData.h"

namespace LittleEngine {
	enum class AssetType {
		Texture,
		Font,
		Sound,
		Music,
	};

	// \brief Auto-constructs a set of generateable asset paths based on input parameters
	struct AssetPaths {
		std::vector<std::string> assetPaths;

		AssetPaths(AssetPaths&&) = default;
		// Single asset's path
		AssetPaths(const std::string& assetPath);
		// Multiple assets' paths
		AssetPaths(std::initializer_list<std::string> assetPaths);
		// Multiple assets' paths with a prefix for each
		AssetPaths(const std::string& pathPrefix, std::initializer_list<std::string> assetPaths);
		// Multiple assets's paths with a prefix for each path, prefix and suffix for each asset
		AssetPaths(const std::string& pathPrefix, int count, const std::string& assetPrefix, const std::string& assetSuffix);

		// Get a random path from vector of paths
		std::string GetRandom() const;
	};

	// \brief Complete data specifying particular set of identical-type Assets
	struct AssetDefinition {
		AssetPaths resourcePaths;
		AssetType type;

		AssetDefinition(AssetDefinition&&) = default;
		AssetDefinition(const AssetDefinition&) = delete;
		// Create an AssetDefinition given a resource type and its AssetPaths
		AssetDefinition(const AssetType& type, AssetPaths&& resourcePaths) : resourcePaths(std::move(resourcePaths)), type(type) {}
	};
	
	// \brief Collection of AssetDefinitions
	struct AssetManifest {
	private:
		std::vector<AssetDefinition> definitions;

	public:
		AssetManifest() = default;
		AssetManifest(std::vector<AssetDefinition>&& assetDefinitions) : definitions(std::move(assetDefinitions)) {}

		// Add definition to the manifest
		void AddDefinition(AssetDefinition&& definition);
		// Create and add an AssetDefinition to the manifest
		void AddDefinition(const AssetType& type, AssetPaths&& resourcePaths);
		void Clear();
		// Convenience callback that iterates through each AssetDefinition
		void ForEach(std::function<void(const AssetDefinition& definition)> Callback) const;
	};

	// \brief Generates AssetManifest from serialised data loaded from a file, given its path
	struct AssetManifestData {
	private:
		AssetManifest manifest;
		
	public:
		AssetManifestData() = default;
		AssetManifestData(AssetManifestData&&) = default;
		AssetManifestData& operator=(AssetManifestData&&) = default;
		// \brief Pass the path to the asset manifest file (.amf)
		AssetManifestData(const std::string& amfPath);

		// \brief Note: this is not a const function! Manifests are designed to move their data
		AssetManifest& GetManifest();
		// \brief Pass the path to the asset manifest file (.amf)
		void Load(const std::string& amfPath);
	};

	// \brief An Asset represents live data in memory that's ready to be used
	class Asset {
	protected:
		std::string m_resourcePath;

	public:
		using Ptr = std::unique_ptr<Asset>;
		Asset() = delete;
		// Path must be relative to the root Asset directory
		Asset(const std::string& path);
		virtual ~Asset();
		const std::string& GetResourcePath() const;

	private:
		Asset(const Asset&) = delete;
		Asset& operator=(const Asset&) = delete;
	};

	// \brief TextureAsset is an image texture copied to VRAM
	class TextureAsset : public Asset {
	private:
		sf::Texture m_sfTexture;

	private:
		// Path must be relative to the root Asset directory
		TextureAsset(const std::string& path);

		// Prevents having to expose texture to code outside SFMLInterface
		friend class AssetManager;
		friend class SpriteRenderable;
	};

	// \brief FontAsset is a font type loaded in RAM
	class FontAsset : public Asset {
	private:
		sf::Font m_sfFont;

	private:
		// Path must be relative to the root Asset directory
		FontAsset(const std::string& path);

		friend class AssetManager;
		friend class TextRenderable;
	};

	// \brief SoundAsset is an audio asset fully loaded in RAM
	class SoundAsset : public Asset {
	private:
		sf::SoundBuffer m_sfSoundBuffer;
		sf::Sound m_sfSound;
		Fixed m_volumeScale = Fixed::One;

	private:
		// Path must be relative to the root Asset directory
		SoundAsset(const std::string& path, const Fixed& volumeScale = Fixed::One);

		friend class AssetManager;
		friend class SoundPlayer;
	};

	// \brief MusicAsset points to a streamed music file on storage
	class MusicAsset : public Asset {
	private:
		sf::Music m_sfMusic;
		Fixed m_volumeScale = Fixed::One;
		bool m_bValid;

	public:
		Fixed GetDurationSeconds() const;

	private:
		// Path must be relative to the root Asset directory
		MusicAsset(const std::string& path, const Fixed& volumeScale = Fixed::One);

		friend class AssetManager;
		friend class MusicPlayer;
		friend class AudioManager;
	};

	// \brief Class that handles all Asset Loading. (Maintains a clearable cache)
	class AssetManager final : public Object {
	private:
		std::unordered_map<std::string, Asset::Ptr> m_loaded;
		std::string m_rootDir;
		FontAsset* m_pDefaultFont;

	public:
		AssetManager(const std::string& rootDir = "");
		~AssetManager();

		// Loads Asset at path. T must derive from Asset!
		// Note: Not meant to be used in hot code!
		template<typename T>
		T* Load(const std::string& path) {
			static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset: check Output window for erroneous call");
			std::string fullPath = GetPath(path);
			auto search = m_loaded.find(fullPath);
			if (search != m_loaded.end()) {
#if defined(LOG_CACHED_ASSET_LOADS)
				Logger::Log(*this, "Found Asset [" + fullPath + "] in cache", Logger::Severity::Debug);
#endif
				return dynamic_cast<T*>(search->second.get());
			}

			Logger::Log(*this, "Loading Asset [" + fullPath + "]", Logger::Severity::Info);
			struct enable_smart : public T { enable_smart(const std::string& path) : T(path) {} };
			std::unique_ptr<T> t = std::make_unique<enable_smart>(fullPath);
			T* pT = t.get();
			m_loaded.emplace(fullPath, std::move(t));
			return pT;
		}
		
		// Note: Not meant to be used in hot code!
		template<typename T>
		std::vector<T*> Load(std::initializer_list<std::string> assetPaths) {
			static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset: check Output window for erroneous call");
			std::vector<T*> vec;
			for (const auto& path : assetPaths) {
				if (T* asset = Load<T>(path)) {
					vec.emplace_back(asset);
				}
			}
			return vec;
		}

		// Note: Not meant to be used in hot code!
		template<typename T>
		std::vector<T*> Load(const std::vector<std::string>& assetPaths) {
			static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset: check Output window for erroneous call");
			std::vector<T*> vec;
			for (const auto& path : assetPaths) {
				if (T* asset = Load<T>(path)) {
					vec.emplace_back(asset);
				}
			}
			return vec;
		}

		// Note: Not meant to be used in hot code!
		void LoadAll(AssetManifest& manifest);
		
		FontAsset* GetDefaultFont() const;

		// Unload all assets
		void UnloadAll();

	private:
		AssetManager(const AssetManager&) = delete;
		AssetManager& operator=(const AssetManager&) = delete;

		std::string GetPath(const std::string& path) const;
	};
}
