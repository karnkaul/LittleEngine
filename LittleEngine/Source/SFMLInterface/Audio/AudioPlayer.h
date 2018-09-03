#pragma once
#include "SFML/Audio.hpp"
#include "Engine/Object.h"
#include "Utils/Fixed.h"
#include "SFMLInterface/Assets.h"
#include "Engine/GameClock.h"

namespace LittleEngine {
	using Fixed = Utils::Fixed;

	// \brief Base class for audio Asset playback
	class AudioPlayer : public Object {
	public:
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
	};

	// \brief Concrete class for Sound playback (uses pre-loaded SoundAsset)
	class SoundPlayer : public AudioPlayer {
	public:
		SoundPlayer(SoundAsset::Ptr soundAsset);
		~SoundPlayer();
		bool SetSoundAsset(SoundAsset::Ptr soundAsset);

		virtual void Play() override;
		virtual void Stop() override;
		virtual void Reset(Fixed seconds = Fixed::Zero) override;
		virtual bool IsPlaying() const override;
		virtual void Tick(Fixed deltaSeconds) override;

	private:
		SoundAsset::Ptr soundAsset;
		bool ApplyParams(bool replaceSound);
	};

	// \brief Concrete class for Music playback (uses streamed MusicAsset)
	class MusicPlayer : public AudioPlayer {
	public:
		MusicPlayer(MusicAsset::Ptr musicAsset);
		~MusicPlayer();
		
		bool SetMusicAsset(MusicAsset::Ptr musicAsset);
		Fixed GetDurationSeconds() const;
		Fixed GetElapsedSeconds() const;
		
		virtual void Play() override;
		virtual void Stop() override;
		virtual void Reset(Fixed seconds = Fixed::Zero) override;
		virtual bool IsPlaying() const override;
		virtual void Tick(Fixed deltaSeconds) override;

	private:
		friend class AudioManager;
		bool ApplyParams(bool replaceMusic);
		GameClock clock;
		MusicAsset::Ptr musicAsset;
	};
}
