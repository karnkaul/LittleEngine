#include "stdafx.h"
#include "SFAudioPlayer.h"
#include "Utils.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "openal32.lib")
#pragma comment(lib, "flac.lib")
#pragma comment(lib, "vorbisenc.lib")
#pragma comment(lib, "vorbisfile.lib")
#pragma comment(lib, "vorbis.lib")
#pragma comment(lib, "ogg.lib")
#if defined(_DEBUG)
#pragma comment(lib, "sfml-audio-s-d.lib")
#pragma comment(lib, "sfml-system-s-d.lib")
#else
#pragma comment(lib, "sfml-audio-s.lib")
#pragma comment(lib, "sfml-system-s.lib")
#endif

namespace LittleEngine
{
AudioPlayer::AudioPlayer() = default;
AudioPlayer::~AudioPlayer() = default;

AudioPlayer::Status AudioPlayer::Cast(sf::Sound::Status status)
{
	switch (status)
	{
	case sf::Sound::Status::Paused:
		return Status::Paused;
	case sf::Sound::Status::Playing:
		return Status::Playing;
	default:
	case sf::Sound::Status::Stopped:
		return Status::Stopped;
	}
}

SoundPlayer::SoundPlayer(SoundAsset* pSoundAsset)
{
	if (pSoundAsset)
		SetSoundAsset(*pSoundAsset);
}

SoundPlayer::~SoundPlayer()
{
	if (IsPlaying())
	{
		m_sfSound.stop();
	}
}

bool SoundPlayer::SetSoundAsset(SoundAsset& soundAsset)
{
	this->m_pSoundAsset = &soundAsset;
	return true;
}

void SoundPlayer::SetDirection(const Fixed& direction)
{
	m_sfSound.setPosition(Maths::Clamp_11(-direction).ToF32(), 0, 0);
}

void SoundPlayer::Play()
{
	if (m_pSoundAsset)
	{
		m_sfSound.setBuffer(m_pSoundAsset->m_sfSoundBuffer);
		ApplyParams();
		m_sfSound.play();
	}
}

void SoundPlayer::Stop()
{
	if (m_pSoundAsset)
	{
		m_sfSound.stop();
	}
}

void SoundPlayer::Pause()
{
	if (m_pSoundAsset && m_status == AudioPlayer::Status::Playing)
	{
		m_sfSound.pause();
		m_status = AudioPlayer::Status::Paused;
	}
}

void SoundPlayer::Resume()
{
	if (m_pSoundAsset)
	{
		if (m_status == AudioPlayer::Status::Paused)
		{
			m_sfSound.play();
		}
	}
}

void SoundPlayer::Reset(Time time)
{
	if (m_pSoundAsset)
	{
		m_sfSound.setPlayingOffset(sf::milliseconds(time.AsMilliseconds()));
	}
}

bool SoundPlayer::IsPlaying() const
{
	return m_pSoundAsset && m_sfSound.getStatus() == sf::SoundSource::Status::Playing;
}

void SoundPlayer::Tick(Time)
{
	m_status = m_pSoundAsset ? Cast(m_sfSound.getStatus()) : Status::NoMedia;
	ApplyParams();
}

bool SoundPlayer::ApplyParams()
{
	if (m_pSoundAsset)
	{
		m_sfSound.setVolume(Maths::Clamp01(m_volume * m_pSoundAsset->m_volumeScale).ToF32() * 100);
		m_sfSound.setLoop(m_bLooping);
		return true;
	}
	return false;
}

MusicPlayer::MusicPlayer(MusicAsset* pMusicAsset)
{
	if (pMusicAsset)
	{
		SetTrack(*pMusicAsset);
	}
}

MusicPlayer::~MusicPlayer()
{
	if (IsPlaying())
	{
		m_pMainTrack->m_sfMusic.stop();
	}
}

bool MusicPlayer::SetTrack(MusicAsset& musicAsset)
{
	this->m_pMainTrack = &musicAsset;
	return true;
}


bool MusicPlayer::HasTrack() const
{
	return m_pMainTrack && m_pMainTrack->m_bValid;
}

Time MusicPlayer::GetDuration() const
{
	if (m_pMainTrack && m_pMainTrack->m_bValid)
	{
		return m_pMainTrack->GetDuration();
	}
	return Time::Zero;
}

Time MusicPlayer::GetElapsed() const
{
	if (IsPlaying())
	{
		return m_clock.GetElapsed();
	}
	return Time::Zero;
}

bool MusicPlayer::IsFading() const
{
	return m_bFadingIn || m_bFadingOut;
}

void MusicPlayer::FadeIn(Time time, const Fixed& targetVolume)
{
	m_bFadingOut = false;
	m_bFadingIn = true;
	m_fadeTime = time;
	this->m_targetVolume = Maths::Clamp01(targetVolume);
	BeginFade();
}

void MusicPlayer::FadeOut(Time time, const Fixed& targetVolume)
{
	m_bFadingIn = false;
	m_bFadingOut = true;
	m_fadeTime = time;
	this->m_targetVolume = Maths::Clamp01(targetVolume);
	BeginFade();
}

void MusicPlayer::EndFade()
{
	m_volume = m_targetVolume;
	if (m_bFadingOut)
	{
		Stop();
		m_pMainTrack = nullptr;
	}
	m_bFadingIn = m_bFadingOut = false;
}

void MusicPlayer::Play()
{
	if (m_pMainTrack && m_pMainTrack->m_bValid)
	{
		m_clock.Restart();
		ApplyParams();
		m_pMainTrack->m_sfMusic.play();
	}
}

void MusicPlayer::Stop()
{
	if (m_pMainTrack)
	{
		m_pMainTrack->m_sfMusic.stop();
	}
}

void MusicPlayer::Pause()
{
	if (m_pMainTrack)
	{
		m_pMainTrack->m_sfMusic.pause();
		m_status = AudioPlayer::Status::Paused;
	}
}

void MusicPlayer::Resume()
{
	if (m_status == AudioPlayer::Status::Paused)
		m_pMainTrack->m_sfMusic.play();
}

void MusicPlayer::Reset(Time time)
{
	if (m_pMainTrack)
	{
		m_clock.Restart();
		m_pMainTrack->m_sfMusic.setPlayingOffset(sf::milliseconds(time.AsMilliseconds()));
	}
}

bool MusicPlayer::IsPlaying() const
{
	return m_pMainTrack && m_pMainTrack->m_sfMusic.getStatus() == sf::SoundSource::Status::Playing;
}

void MusicPlayer::Tick(Time dt)
{
	m_status = m_pMainTrack ? Cast(m_pMainTrack->m_sfMusic.getStatus()) : Status::NoMedia;

	// Process Fade
	if (IsFading())
	{
		this->m_elapsedTime += dt;

		if (this->m_elapsedTime >= m_fadeTime)
		{
			EndFade();
		}

		else
		{
			Fixed ratio =
				Maths::Clamp01(Fixed(m_elapsedTime.AsMilliseconds()) / Fixed(m_fadeTime.AsMilliseconds()));
			if (m_bFadingIn)
			{
				m_volume = (m_targetVolume - m_startVolume) * ratio;
			}
			else
			{
				m_volume = m_startVolume * (Fixed::One - ratio);
			}
			Core::LogOutput("Fading! Volume: " + m_volume.ToString(), Core::LogSeverity::HOT);
		}
	}
	ApplyParams();
}

void MusicPlayer::BeginFade()
{
	if (!IsPlaying())
	{
		m_volume = Fixed(1, 100);
		Play();
	}
	m_startVolume = m_volume;
	m_elapsedTime = Time::Zero;
}

bool MusicPlayer::ApplyParams()
{
	if (m_pMainTrack)
	{
		m_pMainTrack->m_sfMusic.setVolume(Maths::Clamp01(m_volume * m_pMainTrack->m_volumeScale).ToF32() * 100);
		m_pMainTrack->m_sfMusic.setLoop(m_bLooping);
		return true;
	}
	return false;
}
} // namespace LittleEngine
