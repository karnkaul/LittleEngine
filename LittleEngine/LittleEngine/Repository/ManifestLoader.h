#pragma once
#include <functional>
#include "ArchiveReader.h"
#include "SFMLAPI/System/SFTime.h"
#include "SFMLAPI/System/SFAssets.h"

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

		NewAsset(const String& id) : assetID(id)
		{
		}
	};

	Core::ArchiveReader m_archiveReader;
	std::function<void()> m_onDone;
	Vec<NewAsset<TextureAsset>> m_newTextures;
	Vec<NewAsset<FontAsset>> m_newFonts;
	Vec<NewAsset<SoundAsset>> m_newSounds;
	Vec<NewAsset<TextAsset>> m_newTexts;
	class EngineRepository* m_pRepository;
	class MultiJob* m_pMultiJob = nullptr;
	bool m_bCompleted = false;
	bool m_bIdle = false;

public:
#if !SHIPPING
	ManifestLoader(EngineRepository& repository, const String& manifestPath, const std::function<void()>& onDone);
#endif
	ManifestLoader(EngineRepository& repository,
					 const String& archivePath,
					 const String& manifestPath,
					 const std::function<void()>& onDone);

	Fixed GetProgress() const;

private:
	void Tick(Time dt);

	void AddTextureIDs(const AssetIDContainer& IDs);
	void AddFontIDs(const AssetIDContainer& IDs);
	void AddSoundIDs(const AssetIDContainer& IDs);
	void AddTextIDs(const AssetIDContainer& IDs);

	friend class EngineRepository;
};
} // namespace LittleEngine
