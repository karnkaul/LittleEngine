#pragma once
#include <functional>
#include "Core/CoreTypes.h"
#include "LoadHelpers.h"

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

	std::function<void()> m_onDone;
	Vec<NewAsset<class TextureAsset>> m_newTextures;
	Vec<NewAsset<class FontAsset>> m_newFonts;
	Vec<NewAsset<class SoundAsset>> m_newSounds;
	Vec<NewAsset<class TextAsset>> m_newTexts;
	class EngineRepository* m_pRepository;
	class MultiJob* m_pMultiJob = nullptr;
	bool m_bCompleted = false;
	bool m_bIdle = false;

public:
	ManifestLoader(EngineRepository& repository, String manifestPath, std::function<void()> onDone);

	Fixed GetProgress() const;

private:
	void Tick(Time dt);

	void AddTextureIDs(AssetIDContainer IDs);
	void AddFontIDs(AssetIDContainer IDs);
	void AddSoundIDs(AssetIDContainer IDs);
	void AddTextIDs(AssetIDContainer IDs);

	friend class EngineRepository;
};
} // namespace LittleEngine
