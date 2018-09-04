#include "stdafx.h"
#include "Assets.h"
#include "Engine/Logger/Logger.h"

namespace LittleEngine {
	Asset::Asset(const std::string& path) : resourcePath(path) {}

	Asset::~Asset() {
		Logger::Log("[" + resourcePath + "] (Asset) destroyed");
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

	AssetManager::AssetManager(const std::string& rootDir) : Object("AssetManager") {
		this->rootDir = rootDir;
		defaultFont = Load<FontAsset>("main.ttf");
		Logger::Log("AssetManager created");
	}

	FontAsset::Ptr AssetManager::GetDefaultFont() const {
		return defaultFont;
	}

	void AssetManager::UnloadAll() {
		loaded.clear();
		Logger::Log(*this, "AssetManager cleared");
	}

	std::string AssetManager::GetPath(const std::string & path) {
		return rootDir.empty() ? path : rootDir + "/" + path;
	}

	AssetManager::~AssetManager() {
		UnloadAll();
		defaultFont = nullptr;
		Logger::Log(*this, "AssetManager destroyed");
	}
	
	SoundAsset::SoundAsset(const std::string & path, const Fixed& volumeScale) : Asset(path) {
		sfSoundBuffer.loadFromFile(path);
		this->volumeScale = Maths::Clamp01(volumeScale);
	}

	MusicAsset::MusicAsset(const std::string & path, const Fixed& volumeScale) : Asset(path) {
		valid = music.openFromFile(path);
		this->volumeScale = Maths::Clamp01(volumeScale);
	}
	
	Fixed MusicAsset::GetDurationSeconds() const {
		return Fixed(music.getDuration().asSeconds());
	}
}
