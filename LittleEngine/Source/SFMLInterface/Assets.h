#pragma once
#include <string>
#include <unordered_map>
#include "Engine/Object.h"
#include "Engine/Logger/Logger.h"
#include "SFML/Graphics.hpp"

namespace Game {
	// Abstract Wrapper class for all files that can be read as SFML assets
	class Asset {
	public:
		// Path must be relative to the executable
		Asset() = delete;
		Asset(const std::string& path);
		virtual ~Asset();
		const std::string& GetResourcePath() const;
	protected:
		std::string resourcePath;
	private:
		Asset(const Asset&) = delete;
		Asset& operator=(const Asset&) = delete;
	};

	// Concrete wrapper class for SFML Textures
	class TextureAsset : public Asset {
	public:
		// Path must be relative to the executable
		TextureAsset(const std::string& path);
	private:
		// Prevents having to expose texture to code outside SFMLInterface
		friend class SpriteRenderer;
		sf::Texture sfTexture;
	};

	class FontAsset : public Asset {
	public:
		FontAsset(const std::string& path);
	private:
		friend class TextRenderer;
		sf::Font sfFont;
	};

	// Class that handles all Asset Loading
	// (Maintains a clearable cache)
	class AssetManager final : public Object {
	public:
		AssetManager();
		~AssetManager();

		// Loads Asset at path. T must derive from Asset!
		template<typename T>
		std::shared_ptr<T> LoadAsset(const std::string& path) {
			static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset: check Output window for erroneous call");
			auto search = loaded.find(path);
			if (search != loaded.end()) {
				Logger::Log("AssetManager", "Found Asset [" + path + "] in cache", Logger::Severity::Debug);
				std::shared_ptr<Asset> asset = search->second;
				return std::dynamic_pointer_cast<T>(asset);
			}

			Logger::Log("AssetManager", "Loading Asset [" + path + "]", Logger::Severity::Debug);
			std::shared_ptr<T> t_ptr = std::make_shared<T>(path);
			std::shared_ptr<Asset> t_asset = std::dynamic_pointer_cast<Asset>(t_ptr);
			loaded.insert(std::pair<std::string, std::shared_ptr<Asset> >(path, t_asset));
			return t_ptr;
		}
		
		FontAsset& GetDefaultFont() const;
		void LoadAllTextures(std::initializer_list<std::string> texturePaths);
		void Clear();

	private:
		AssetManager(const AssetManager&) = delete;
		AssetManager& operator=(const AssetManager&) = delete;
		std::shared_ptr<FontAsset> defaultFont;
		std::unordered_map<std::string, std::shared_ptr<Asset> > loaded;
	};
}