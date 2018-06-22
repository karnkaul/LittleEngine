#include "stdafx.h"
#include "Assets.h"
#include "Engine/Logger/Logger.h"

namespace Game {
	Asset::Asset(const std::string& path) : resourcePath(path) {}

	Asset::~Asset() {
		Logger::Log("Asset pointing to [" + resourcePath + "] destroyed");
	}

	const std::string & Asset::GetResourcePath() const {
		return resourcePath;
	}

	TextureAsset::TextureAsset(const std::string& path) : Asset(path) {
		if (!sfTexture.loadFromFile(resourcePath)) {
			Logger::Log("Could not load texture from " + resourcePath + "!", Logger::Severity::Error);
		}
	}

	FontAsset::FontAsset(const std::string & path) : Asset(path) {
		if (!sfFont.loadFromFile(resourcePath)) {
			Logger::Log("Could not load font from " + resourcePath + "!", Logger::Severity::Error);
		}
	}

	AssetManager::AssetManager() : Object("AssetManager") {
		Logger::Log("AssetManager created");
		defaultFont = LoadAsset<FontAsset>("Assets/main.ttf");
	}

	void AssetManager::LoadAllTextures(std::initializer_list<std::string> texturePaths) {
		for (const auto& path : texturePaths) {
			LoadAsset<TextureAsset>(path);
		}
	}

	FontAsset::Ptr AssetManager::GetDefaultFont() const {
		return defaultFont;
	}

	void AssetManager::Clear() {
		loaded.clear();
		Logger::Log(*this, "AssetManager cleared");
	}

	AssetManager::~AssetManager() {
		Clear();
		defaultFont = nullptr;
		Logger::Log(*this, "AssetManager destroyed");
	}
}
