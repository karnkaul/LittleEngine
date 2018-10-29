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
	using Fixed = GameUtils::Fixed;
	using GData = GameUtils::GData;

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
		AssetType type;
		AssetPaths resourcePaths;

		AssetDefinition(AssetDefinition&&) = default;
		AssetDefinition(const AssetDefinition&) = delete;
		// Create an AssetDefinition given a resource type and its AssetPaths
		AssetDefinition(const AssetType& type, AssetPaths&& resourcePaths) : type(type), resourcePaths(std::move(resourcePaths)) {}
	};
	
	// \brief Collection of AssetDefinitions
	struct AssetManifest {
		AssetManifest() = default;
		AssetManifest(std::vector<AssetDefinition>&& assetDefinitions) : definitions(std::move(assetDefinitions)) {}

		// Add definition to the manifest
		void AddDefinition(AssetDefinition&& definition);
		// Create and add an AssetDefinition to the manifest
		void AddDefinition(const AssetType& type, AssetPaths&& resourcePaths);
		void Clear();
		// Convenience callback that iterates through each AssetDefinition
		void ForEach(std::function<void(const AssetDefinition& definition)> Callback) const;

	private:
		std::vector<AssetDefinition> definitions;
	};

	// \brief Generates AssetManifest from serialised data loaded from a file, given its path
	struct AssetManifestData {
		AssetManifestData() = default;
		AssetManifestData(AssetManifestData&&) = default;
		AssetManifestData& operator=(AssetManifestData&&) = default;
		// \brief Pass the path to the asset manifest file (.amf)
		AssetManifestData(const std::string& amfPath);

		// \brief Note: this is not a const function! Manifests are designed to move their data
		AssetManifest& GetManifest();
		// \brief Pass the path to the asset manifest file (.amf)
		void Load(const std::string& amfPath);

	private:
		AssetManifest manifest;
	};

	// \brief Abstract Wrapper class for all files that can be read as SFML assets
	class Asset {
	public:
		Asset() = delete;
		// Path must be relative to the root Asset directory
		Asset(const std::string& path);
		virtual ~Asset();
		const std::string& GetResourcePath() const;
	protected:
		std::string resourcePath;
	private:
		Asset(const Asset&) = delete;
		Asset& operator=(const Asset&) = delete;
	};

	// \brief Concrete wrapper class for SFML Textures
	class TextureAsset : public Asset {
	public:
		using Ptr = std::shared_ptr<TextureAsset>;
	private:
		// Prevents having to expose texture to code outside SFMLInterface
		friend class AssetManager;
		friend class SpriteRenderer;

		// Path must be relative to the root Asset directory
		TextureAsset(const std::string& path);
		sf::Texture sfTexture;
	};

	// \brief Concrete wrapper for SFML Fonts
	class FontAsset : public Asset {
	public:
		using Ptr = std::shared_ptr<FontAsset>;
	private:
		friend class AssetManager;
		friend class TextRenderer;

		// Path must be relative to the root Asset directory
		FontAsset(const std::string& path);
		sf::Font sfFont;
	};

	// \brief Concrete wrapper for SFML Sounds
	class SoundAsset : public Asset {
	public:
		using Ptr = std::shared_ptr<SoundAsset>;
		
	private:
		friend class AssetManager;
		friend class SoundPlayer;

		sf::SoundBuffer sfSoundBuffer;
		sf::Sound sfSound;
		Fixed volumeScale = Fixed::One;

		// Path must be relative to the root Asset directory
		SoundAsset(const std::string& path, const Fixed& volumeScale = Fixed::One);
	};

	// \brief Concrete wrapper for SFML Music
	class MusicAsset : public Asset {
	public:
		using Ptr = std::shared_ptr<MusicAsset>;
		Fixed GetDurationSeconds() const;

	private:
		friend class AssetManager;
		friend class MusicPlayer;
		friend class AudioManager;

		sf::Music music;
		Fixed volumeScale = Fixed::One;
		bool valid;

		// Path must be relative to the root Asset directory
		MusicAsset(const std::string& path, const Fixed& volumeScale = Fixed::One);
	};

	// \brief Class that handles all Asset Loading. (Maintains a clearable cache)
	class AssetManager final : public Object {
	public:
		AssetManager(const std::string& rootDir = "");
		~AssetManager();

		// Loads Asset at path. T must derive from Asset!
		// Note: Not meant to be used in hot code unless Asset is already cached!
		template<typename T>
		std::shared_ptr<T> Load(const std::string& path) {
			static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset: check Output window for erroneous call");
			std::string fullPath = GetPath(path);
			auto search = loaded.find(fullPath);
			if (search != loaded.end()) {
#if defined(LOG_CACHED_ASSET_LOADS)
				Logger::Log(*this, "Found Asset [" + fullPath + "] in cache", Logger::Severity::Debug);
#endif
				std::shared_ptr<Asset> asset = search->second;
				return std::dynamic_pointer_cast<T>(asset);
			}

			Logger::Log(*this, "Loading Asset [" + fullPath + "]", Logger::Severity::Info);
			struct enable_shared : public T { enable_shared(const std::string& path) : T(path) {} };
			std::shared_ptr<T> t_ptr = std::make_shared<enable_shared>(fullPath);
			loaded.emplace(fullPath, t_ptr);
			return t_ptr;
		}
		
		// Note: Not meant to be used in hot code unless Asset is already cached!
		template<typename T>
		std::vector<std::shared_ptr<T>> Load(std::initializer_list<std::string> assetPaths) {
			static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset: check Output window for erroneous call");
			std::vector<std::shared_ptr<T>> vec;
			for (const auto& path : assetPaths) {
				vec.emplace_back(Load<T>(path));
			}
			return vec;
		}

		// Note: Not meant to be used in hot code unless Asset is already cached!
		template<typename T>
		std::vector<std::shared_ptr<T>> Load(const std::vector<std::string>& assetPaths) {
			static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset: check Output window for erroneous call");
			std::vector<std::shared_ptr<T>> vec;
			for (const auto& path : assetPaths) {
				vec.emplace_back(Load<T>(path));
			}
			return vec;
		}

		void LoadAll(AssetManifest& manifest);
		
		FontAsset::Ptr GetDefaultFont() const;

		// Unload all assets
		void UnloadAll();

	private:
		std::string rootDir;
		std::string GetPath(const std::string& path);
		std::unordered_map<std::string, std::shared_ptr<Asset>> loaded;
		FontAsset::Ptr defaultFont;

		AssetManager(const AssetManager&) = delete;
		AssetManager& operator=(const AssetManager&) = delete;
	};
}
