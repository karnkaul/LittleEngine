#pragma once
#include "Core/CoreTypes.h"
#include "AssetManifest.h"

namespace Core
{
class MultiJob;
}

namespace LittleEngine
{
class ManifestLoader final
{
private:
	template <typename T>
	struct NewAsset
	{
		UPtr<T> asset;
		String assetID;

		NewAsset(String id) : assetID(std::move(id))
		{
		}
	};

	Task m_onDone;
	Vec<NewAsset<class TextureAsset>> m_newTextures;
	Vec<NewAsset<class FontAsset>> m_newFonts;
	Vec<NewAsset<class SoundAsset>> m_newSounds;
	Vec<NewAsset<class TextAsset>> m_newTexts;
	class LERepository* m_pRepository;
	Core::MultiJob* m_pMultiJob = nullptr;
	bool m_bCompleted = false;
	bool m_bIdle = false;
#if ENABLED(FILESYSTEM_ASSETS)
	bool m_bManifestFilePresent = false;
#endif

public:
	ManifestLoader(LERepository& repository, String manifestPath, Task onDone);

	Fixed GetProgress() const;

private:
	void Tick(Time dt);

	void AddTextureIDs(AssetIDContainer IDs);
	void AddFontIDs(AssetIDContainer IDs);
	void AddSoundIDs(AssetIDContainer IDs);
	void AddTextIDs(AssetIDContainer IDs);

	friend class LERepository;
};
} // namespace LittleEngine
