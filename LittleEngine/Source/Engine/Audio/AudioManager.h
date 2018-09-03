#pragma once
#include <vector>
#include "SFMLInterface/Audio/AudioPlayer.h"

namespace LittleEngine {
	class Engine;

	// \brief Handler for all SFX and Music playback
	class AudioManager final : public Object {
	public:
		AudioManager(Engine& engine);
		~AudioManager();

		// Returns true if asset is loaded successfully
		bool PlaySFX(const std::string& path, const Fixed& volume = 80, bool loop = false);
		bool IsSFXPlaying() const;

		// Returns true if asset is loaded successfully
		bool PlayMusic(const std::string& path, const Fixed& volume = 80, bool loop = true);
		bool IsMusicPlaying() const;
		void StopMusic();
		
		// Stop all audio and flush all SFX players
		void Clear();

	private:
		friend class Engine;

		std::vector<std::unique_ptr<SoundPlayer>> sfxPlayers;
		MusicPlayer musicPlayer;
		Engine* engine;

		AssetManager& GetAssetManager();
		SoundPlayer& GetOrCreateSFXPlayer();

		// Engine to call
		void Tick(Fixed deltaSeconds);
	};
}
