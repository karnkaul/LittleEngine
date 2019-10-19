#pragma once
#include "Core/CoreTypes.h"
#include "AssetManifest.h"

namespace Core
{
class JobCatalog;
}

namespace LittleEngine
{
class ManifestLoader final
{
private:
	template <typename T>
	struct NewAsset
	{
		std::unique_ptr<T> asset;
		std::string assetID;

		NewAsset(std::string id) : assetID(std::move(id)) {}
	};

	Task m_onDone;
	std::string m_manifestPath;
	std::vector<NewAsset<class TextureAsset>> m_newTextures;
	std::vector<NewAsset<class FontAsset>> m_newFonts;
	std::vector<NewAsset<class SoundAsset>> m_newSounds;
	std::vector<NewAsset<class TextAsset>> m_newTexts;
	class LERepository* m_pRepository;
	Core::JobCatalog* m_pJobCatalog = nullptr;
	bool m_bUnloading = false;
	bool m_bCompleted = false;
	bool m_bIdle = false;
#if ENABLED(FILESYSTEM_ASSETS)
	bool m_bManifestFilePresent = false;
#endif

public:
	ManifestLoader(LERepository& repository, std::string manifestPath, Task onDone, bool bUnload);

	Fixed Progress() const;

private:
	void Tick(Time dt);

	void AddTextureIDs(AssetIDContainer IDs);
	void AddFontIDs(AssetIDContainer IDs);
	void AddSoundIDs(AssetIDContainer IDs);
	void AddTextIDs(AssetIDContainer IDs);

	friend class LERepository;
};
} // namespace LittleEngine
