#pragma once
#include <vector>
#include "SFMLInterface/Audio/AudioPlayer.h"

namespace LittleEngine {
	class Engine;

	// \brief Handler for all SFX and Music playback
	class AudioManager final : public Object {
	public:
		using Fixed = GameUtils::Fixed;

		AudioManager(Engine& engine);
		~AudioManager();

		// Returns nullptr if asset could not be loaded 
		SoundPlayer* PlaySFX(const std::string& path, const Fixed& volume = Fixed::One, const Fixed& direction = Fixed::Zero);
		bool IsSFXPlaying() const;

		// Returns true if asset is loaded successfully
		bool PlayMusic(const std::string& path, const Fixed& volume = Fixed::One, const Fixed& fadeSeconds = Fixed::OneHalf, bool loop = true);
		bool IsMusicPlaying() const;
		void StopMusic(const Fixed& fadeSeconds = Fixed::Zero);
		void SwitchTrack(const std::string& path, const Fixed& volume = Fixed::One, const Fixed& fadeSeconds = Fixed::One);
		void SetMusicVolume(const Fixed& volume);
		
		// Stop all audio and flush all SFX players
		void Clear(bool immediate = true);

	private:
		friend class Engine;

		// \brief Helper to store next track until current has faded out
		struct SwitchTrackRequest {
			MusicAsset::Ptr newTrack = nullptr;
			Fixed fadeSeconds = Fixed::Zero;
			Fixed targetVolume = Fixed::Zero;
			bool fadingOldTrack = false;
			bool fadingNewTrack = false;

			SwitchTrackRequest(MusicAsset::Ptr newTrack, const Fixed& fadeSeconds, const Fixed& targetVolume);
		};

		std::vector<std::unique_ptr<SoundPlayer>> sfxPlayers;
		std::unique_ptr<SwitchTrackRequest> switchTrackRequest = nullptr;
		MusicPlayer musicPlayerA;
		MusicPlayer musicPlayerB;
		bool sideA = true;
		Engine* engine;

		AssetManager& GetAssetManager();
		SoundPlayer& GetOrCreateSFXPlayer();
		MusicPlayer& GetActivePlayer();
		MusicPlayer& GetStandbyPlayer();
		const MusicPlayer& GetActivePlayer() const;
		const MusicPlayer& GetStandbyPlayer() const;

		// Engine to call
		void Tick(Fixed deltaTime);
	};
}
