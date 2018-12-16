#pragma once
#include "SFML/Audio.hpp"
#include "Engine/Object.h"
#include "Fixed.h"
#include "SFMLInterface/Assets.h"
#include "Engine/GameClock.h"

namespace LittleEngine {
	using Fixed = GameUtils::Fixed;

	// \brief Base class for audio Asset playback
	class AudioPlayer : public Object {
	public:
		enum class Status { NoMedia, Stopped, Playing, Paused };
		
	public:
		Fixed m_volume = Fixed(80, 100);
		Status m_status;
		bool m_bLooping;

	public:
		virtual ~AudioPlayer();
		virtual void Play() = 0;
		virtual void Stop() = 0;
		virtual void Reset(Fixed seconds = Fixed::Zero) = 0;
		virtual bool IsPlaying() const = 0;
		virtual void Tick(Fixed deltaSeconds) = 0;

	protected:
		AudioPlayer(const std::string& name);
		Status Cast(sf::Sound::Status status);
		virtual bool ApplyParams() = 0;
	};

	// \brief Concrete class for Sound playback (uses pre-loaded SoundAsset)
	class SoundPlayer : public AudioPlayer {
	private:
		SoundAsset* m_pSoundAsset;

	public:
		SoundPlayer(SoundAsset* pSoundAsset = nullptr);
		~SoundPlayer();

		bool SetSoundAsset(SoundAsset& soundAsset);
		void SetDirection(const Fixed& direction);

		virtual void Play() override;
		virtual void Stop() override;
		virtual void Reset(Fixed seconds = Fixed::Zero) override;
		virtual bool IsPlaying() const override;
		virtual void Tick(Fixed deltaSeconds) override;

	private:
		virtual bool ApplyParams() override;
	};

	// \brief Concrete class for Music playback (uses streamed MusicAsset)
	class MusicPlayer : public AudioPlayer {
	private:
		GameClock m_clock;
		Fixed m_fadeSeconds = Fixed::Zero;
		Fixed m_elapsedSeconds = Fixed::Zero;
		Fixed m_targetVolume = Fixed::One;
		Fixed m_startVolume = Fixed::One;
		MusicAsset* m_pMainTrack = nullptr;
		bool m_bFadingIn = false;
		bool m_bFadingOut = false;

	public:
		MusicPlayer(MusicAsset* pMusicAsset = nullptr);
		~MusicPlayer();
		
		bool SetTrack(MusicAsset& track);
		Fixed GetDurationSeconds() const;
		Fixed GetElapsedSeconds() const;
		bool IsFading() const;
		void FadeIn(const Fixed& timeSeconds, const Fixed& targetVolume = Fixed::One);
		void FadeOut(const Fixed& timeSeconds, const Fixed& targetVolume = Fixed::Zero);
		void EndFade();
		
		virtual void Play() override;
		virtual void Stop() override;
		virtual void Reset(Fixed seconds = Fixed::Zero) override;
		virtual bool IsPlaying() const override;
		virtual void Tick(Fixed deltaSeconds) override;

	private:
		void BeginFade();

		virtual bool ApplyParams() override;

		friend class AudioManager;
	};
}
