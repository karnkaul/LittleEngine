#pragma once
#include "Core/CoreTypes.h"

namespace sf
{
class Music;
class Sound;
} // namespace sf

namespace LittleEngine
{
// \brief Base class for audio Asset playback
class AudioPlayer
{
public:
	enum class Status : u8
	{
		NoMedia = 0,
		Stopped,
		Playing,
		Paused
	};

	enum class Mix
	{
		Master,
		Music,
		SFX
	};

public:
	static Array<Fixed, 3> s_mixVols;
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

	static Fixed MixVolume(Mix mix, Fixed nVol);

protected:
	AudioPlayer();
	virtual bool ApplyParams() = 0;
};

// \brief Concrete class for Sound playback (uses pre-loaded SoundAsset)
class SoundPlayer : public AudioPlayer
{
private:
	UPtr<sf::Sound> m_uSFSound;
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
	UPtr<sf::Music> m_uSFMusic;
	bool m_bFadingIn = false;
	bool m_bFadingOut = false;
	bool m_bPauseOnFadeOut = false;

public:
	MusicPlayer();
	~MusicPlayer() override;

	bool SetTrack(String path);
	Time Duration() const;
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

	friend class LEAudio;
};
} // namespace LittleEngine
