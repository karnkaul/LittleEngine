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
		Status status;

		Fixed volume = Fixed(80, 100);
		bool looping;

		virtual ~AudioPlayer();
		virtual void Play() = 0;
		virtual void Stop() = 0;
		virtual void Reset(Fixed seconds = Fixed::Zero) = 0;
		virtual bool IsPlaying() const = 0;
		virtual void Tick(Fixed deltaSeconds) = 0;

	protected:
		AudioPlayer(const std::string& name);
		virtual bool ApplyParams() = 0;
		Status Convert(sf::Sound::Status status);
	};

	// \brief Concrete class for Sound playback (uses pre-loaded SoundAsset)
	class SoundPlayer : public AudioPlayer {
	public:
		SoundPlayer(SoundAsset::Ptr soundAsset);
		~SoundPlayer();
		bool SetSoundAsset(SoundAsset::Ptr soundAsset);

		void SetDirection(const Fixed& direction);
		virtual void Play() override;
		virtual void Stop() override;
		virtual void Reset(Fixed seconds = Fixed::Zero) override;
		virtual bool IsPlaying() const override;
		virtual void Tick(Fixed deltaSeconds) override;

	private:
		SoundAsset::Ptr soundAsset;
		virtual bool ApplyParams() override;
	};

	// \brief Concrete class for Music playback (uses streamed MusicAsset)
	class MusicPlayer : public AudioPlayer {
	public:
		MusicPlayer(MusicAsset::Ptr musicAsset = nullptr);
		~MusicPlayer();
		
		bool SetTrack(MusicAsset::Ptr track);
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
		friend class AudioManager;
		GameClock clock;
		MusicAsset::Ptr mainTrack = nullptr;
		Fixed fadeSeconds = Fixed::Zero;
		Fixed elapsedSeconds = Fixed::Zero;
		Fixed targetVolume = Fixed::One;
		Fixed startVolume = Fixed::One;
		bool fadingIn = false;
		bool fadingOut = false;

		void BeginFade();

		virtual bool ApplyParams() override;
	};
}
