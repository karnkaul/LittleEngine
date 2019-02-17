#pragma once
#include "SFMLAPI/System/SFTime.h"
#include "SFMLAPI/System/SFAssets.h"

namespace LittleEngine
{
class AsyncAssetLoader final
{
private:
	template <typename T>
	struct NewAsset
	{
		UPtr<T> asset;
		String assetPath;

		NewAsset(const String& path) : assetPath(path)
		{
		}
	};

	Function(void()) m_onDone;
	Vector<NewAsset<TextureAsset>> m_newTextures;
	Vector<NewAsset<FontAsset>> m_newFonts;
	Vector<NewAsset<SoundAsset>> m_newSounds;
	class EngineRepository* m_pRepository;
	class MultiJob* m_pMultiJob = nullptr;
	bool m_bCompleted = false;
	bool m_bIdle = false;

public:
	AsyncAssetLoader(EngineRepository& repository, const String& manifestPath, Function(void()) onDone);

	Fixed GetProgress() const;

private:
	void Tick(Time dt);

	void AddTexturePaths(const AssetPaths& paths);
	void AddFontPaths(const AssetPaths& paths);
	void AddSoundPaths(const AssetPaths& paths);

	friend class EngineRepository;
};
} // namespace LittleEngine
