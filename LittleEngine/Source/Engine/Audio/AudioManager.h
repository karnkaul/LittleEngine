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
		bool PlaySFX(const std::string& path, const Fixed& volume = Fixed::One, bool loop = false);
		bool IsSFXPlaying() const;

		// Returns true if asset is loaded successfully
		bool PlayMusic(const std::string& path, const Fixed& volume = Fixed::One, const Fixed& fadeSeconds = Fixed::Half, bool loop = true);
		bool IsMusicPlaying() const;
		void StopMusic(const Fixed& fadeSeconds = Fixed::Zero);
		void SwitchTrack(const std::string& path, const Fixed& volume = Fixed::One, const Fixed& fadeSeconds = Fixed::One);
		void SetMusicVolume(const Fixed& volume);
		
		// Stop all audio and flush all SFX players
		void Clear(bool immediate = true);

	private:
		friend class Engine;

		// \brief Helper to fade music over time
		struct MusicFader {
			Fixed fadeSeconds = Fixed::Zero;
			Fixed elapsedSeconds = Fixed::Zero;
			Fixed targetVolume = Fixed::One;
			Fixed startVolume = Fixed::One;
			bool fadingIn = false;
			bool fadingOut = false;

			MusicFader(MusicPlayer& player);

			bool IsFading() const;
			void FadeIn(const Fixed& timeSeconds, const Fixed& targetVolume = Fixed::One);
			void FadeOut(const Fixed& timeSeconds, const Fixed& targetVolume = Fixed::Zero);
			void EndFade();
			void Tick(const Fixed& deltaTime);

		private:
			void BeginFade();
			MusicPlayer* player;
		};

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
		MusicPlayer musicPlayer;
		MusicFader musicFader;
		Engine* engine;

		AssetManager& GetAssetManager();
		SoundPlayer& GetOrCreateSFXPlayer();

		// Engine to call
		void Tick(Fixed deltaTime);
	};
}
