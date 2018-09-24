#include "stdafx.h"
#include "Assets.h"
#include "Engine/Logger/Logger.h"

namespace LittleEngine {
	Asset::Asset(const std::string& path) : resourcePath(path) {}

	Asset::~Asset() {
		Logger::Log("[" + resourcePath + "] (Asset) destroyed", Logger::Severity::Debug);
	}

	const std::string & Asset::GetResourcePath() const {
		return resourcePath;
	}

	TextureAsset::TextureAsset(const std::string& path) : Asset(path) {
		if (!sfTexture.loadFromFile(resourcePath)) {
			Logger::Log("Could not load texture from [" + resourcePath + "]!", Logger::Severity::Error);
		}
	}

	FontAsset::FontAsset(const std::string & path) : Asset(path) {
		if (!sfFont.loadFromFile(resourcePath)) {
			Logger::Log("Could not load font from [" + resourcePath + "]!", Logger::Severity::Error);
		}
	}

	SoundAsset::SoundAsset(const std::string & path, const Fixed& volumeScale) : Asset(path), volumeScale(Maths::Clamp01(volumeScale)) {
		if (!sfSoundBuffer.loadFromFile(path)) {
			Logger::Log("Could not load sound from [" + resourcePath + "]!", Logger::Severity::Error);
		}
	}

	MusicAsset::MusicAsset(const std::string & path, const Fixed& volumeScale) : Asset(path), volumeScale(Maths::Clamp01(volumeScale)) {
		valid = music.openFromFile(path);
		if (!valid) {
			Logger::Log("Could not load music from [" + resourcePath + "]!", Logger::Severity::Error);
		}
	}

	Fixed MusicAsset::GetDurationSeconds() const {
		return Fixed(music.getDuration().asSeconds());
	}

	AssetPaths::AssetPaths(const std::string & assetPath) {
		assetPaths.push_back(assetPath);
	}

	AssetPaths::AssetPaths(std::initializer_list<std::string> assetPaths) {
		for (const auto& path : assetPaths) {
			this->assetPaths.push_back(path);
		}
	}

	AssetPaths::AssetPaths(const std::string & pathPrefix, std::initializer_list<std::string> assetPaths) {
		std::string prefix = pathPrefix.empty() ? "" : pathPrefix + "/";
		for (const auto& path : assetPaths) {
			this->assetPaths.emplace_back(prefix + path);
		}
	}

	AssetPaths::AssetPaths(const std::string & pathPrefix, int count, const std::string & assetPrefix, const std::string& assetSuffix) {
		std::string prefix = pathPrefix.empty() ? "" : pathPrefix + "/";
		for (int i = 0; i < count; ++i) {
			std::string suffix = (i < 10) ? "0" + std::to_string(i) : std::to_string(i);
			suffix += assetSuffix;
			this->assetPaths.push_back(prefix + assetPrefix + suffix);
		}
	}

	std::string AssetPaths::GetRandom() const {
		size_t index = Maths::Random::Range(0, static_cast<int>(assetPaths.size()));
		return assetPaths[index];
	}

	AssetManager::AssetManager(const std::string& rootDir) : Object("AssetManager"), rootDir(rootDir) {
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
		defaultFont = nullptr;
		for (const auto& iter : loaded) {
			if (iter.second.use_count() > 1) {
				Logger::Log(*this, "Unreleased asset! [" + iter.second->GetResourcePath() + "]", Logger::Severity::Warning);
			}
		}
		UnloadAll();
		Logger::Log(*this, "AssetManager destroyed");
	}
}
