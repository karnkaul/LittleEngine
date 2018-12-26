#pragma once
#include <vector>
#include "SFMLInterface/Audio/AudioPlayer.h"

namespace LittleEngine {
	class Engine;

	// \brief Handler for all SFX and Music playback
	class AudioManager final : public Object {
	private:
		// \brief Helper to store next track until current has faded out
		struct SwitchTrackRequest {
			Fixed fadeSeconds;
			Fixed targetVolume;
			MusicAsset* newTrack;
			bool bFadingOldTrack = false;
			bool bFadingNewTrack = false;

			SwitchTrackRequest(MusicAsset& newTrack, const Fixed& fadeSeconds, const Fixed& targetVolume) : fadeSeconds(fadeSeconds), targetVolume(targetVolume), newTrack(&newTrack) {}
		};

	private:
		MusicPlayer m_musicPlayerA;
		MusicPlayer m_musicPlayerB;
		std::vector<std::unique_ptr<SoundPlayer>> m_sfxPlayers;
		std::unique_ptr<SwitchTrackRequest> m_uSwitchTrackRequest = nullptr;
		Engine* m_pEngine;
		bool m_bSideA = true;

	public:
		using Fixed = GameUtils::Fixed;

		AudioManager(Engine& engine);
		~AudioManager();

		// Returns nullptr if asset could not be loaded 
		SoundPlayer* PlaySFX(const std::string& path, const Fixed& volume = Fixed::One, const Fixed& direction = Fixed::Zero, bool bLoop = false);
		SoundPlayer* PlaySFX(SoundAsset& sound, const Fixed& volume = Fixed::One, const Fixed& direction = Fixed::Zero, bool bLoop = false);
		bool IsSFXPlaying() const;

		// Returns true if asset is loaded successfully
		bool PlayMusic(const std::string& path, const Fixed& volume = Fixed::One, const Fixed& fadeSeconds = Fixed::OneHalf, bool bLoop = true);
		bool IsMusicPlaying() const;
		void StopMusic(const Fixed& fadeSeconds = Fixed::Zero);
		void SwitchTrack(const std::string& path, const Fixed& volume = Fixed::One, const Fixed& fadeSeconds = Fixed::One);
		void SetMusicVolume(const Fixed& volume);
		
		// Stop all audio and flush all SFX players
		void Clear(bool bImmediate = true);

	private:
		AssetManager& GetAssetManager();
		SoundPlayer& GetOrCreateSFXPlayer();
		MusicPlayer& GetActivePlayer();
		MusicPlayer& GetStandbyPlayer();
		const MusicPlayer& GetActivePlayer() const;
		const MusicPlayer& GetStandbyPlayer() const;

		// Engine to call
		void Tick(Fixed deltaMS);

		friend class Engine;
	};
}
