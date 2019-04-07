#pragma once
#include "CoreTypes.h"
#include "SimpleTime.h"

namespace sf
{
class Music;
}

namespace LittleEngine
{
// \brief Base class for audio Asset playback
class AudioPlayer
{
public:
	enum class Status
	{
		NoMedia,
		Stopped,
		Playing,
		Paused
	};

public:
	Fixed m_volume = Fixed(80, 100);
	Status m_status = Status::NoMedia;
	bool m_bLooping = false;

public:
	virtual ~AudioPlayer();
	virtual void Play() = 0;
	virtual void Stop() = 0;
	virtual void Pause() = 0;
	virtual void Resume() = 0;
	virtual void Reset(Time time = Time::Zero) = 0;
	virtual bool IsPlaying() const = 0;
	virtual void Tick(Time dt) = 0;

protected:
	AudioPlayer();
	Status Cast(sf::Sound::Status status);
	virtual bool ApplyParams() = 0;
};

// \brief Concrete class for Sound playback (uses pre-loaded SoundAsset)
class SoundPlayer : public AudioPlayer
{
private:
	sf::Sound m_sfSound;
	class SoundAsset* m_pSoundAsset = nullptr;

public:
	SoundPlayer(SoundAsset* pSoundAsset = nullptr);
	~SoundPlayer() override;

	bool SetSoundAsset(SoundAsset& soundAsset);
	void SetDirection(Fixed direction);

	void Play() override;
	void Stop() override;
	void Pause() override;
	void Resume() override;
	void Reset(Time time = Time::Zero) override;
	bool IsPlaying() const override;
	void Tick(Time dt) override;

private:
	bool ApplyParams() override;
};

// \brief Concrete class for Music playback (uses streamed MusicAsset)
class MusicPlayer : public AudioPlayer
{
private:
	Time m_fadeTime;
	Time m_elapsedTime;
	Fixed m_targetVolume = Fixed::One;
	Fixed m_startVolume = Fixed::One;
	UPtr<class GameClock> m_uClock;
	UPtr<sf::Music> m_uSFMusic;
	bool m_bFadingIn = false;
	bool m_bFadingOut = false;

public:
	MusicPlayer();
	~MusicPlayer() override;

	bool SetTrack(const String& path);
	Time GetDuration() const;
	Time GetElapsed() const;
	bool IsFading() const;
	void FadeIn(Time time, Fixed targetVolume = Fixed::One);
	void FadeOut(Time time, Fixed targetVolume = Fixed::Zero);
	void EndFade();

	bool IsPaused() const;
	void Play() override;
	void Stop() override;
	void Pause() override;
	void Resume() override;
	void Reset(Time time = Time::Zero) override;
	bool IsPlaying() const override;
	void Tick(Time dt) override;

private:
	void BeginFade();

	bool ApplyParams() override;

	friend class EngineAudio;
};
} // namespace LittleEngine
