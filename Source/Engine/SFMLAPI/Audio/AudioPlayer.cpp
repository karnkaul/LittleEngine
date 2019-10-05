#include "SFML/Audio.hpp"
#include "Core/Utils.h"
#include "Core/Logger.h"
#include "AudioPlayer.h"
#include "SFMLAPI/System/Assets.h"

namespace LittleEngine
{
namespace
{
AudioPlayer::Status Cast(sf::Sound::Status status)
{
	switch (status)
	{
	case sf::Sound::Status::Paused:
		return AudioPlayer::Status::Paused;
	case sf::Sound::Status::Playing:
		return AudioPlayer::Status::Playing;
	default:
	case sf::Sound::Status::Stopped:
		return AudioPlayer::Status::Stopped;
	}
}
} // namespace

std::array<Fixed, 3> AudioPlayer::s_mixVols = {1, 1, 1};

AudioPlayer::AudioPlayer() = default;
AudioPlayer::~AudioPlayer() = default;

Fixed AudioPlayer::MixVolume(Mix mix, Fixed nVol)
{
	return Maths::Clamp01(nVol) * Maths::Clamp01(s_mixVols[ToIdx(mix)]) * Maths::Clamp01(s_mixVols[ToIdx(Mix::Master)]);
}

SoundPlayer::SoundPlayer(SoundAsset* pSoundAsset)
{
	m_uSFSound = MakeUnique<sf::Sound>();
	if (pSoundAsset)
	{
		SetSoundAsset(*pSoundAsset);
	}
}

SoundPlayer::~SoundPlayer()
{
	if (IsPlaying())
	{
		m_uSFSound->stop();
	}
}

bool SoundPlayer::SetSoundAsset(SoundAsset& soundAsset)
{
	this->m_pSoundAsset = &soundAsset;
	return true;
}

void SoundPlayer::SetDirection(Fixed direction)
{
	m_uSFSound->setPosition(Maths::Clamp_11(-direction).ToF32(), 0, 0);
}

void SoundPlayer::Play()
{
	if (m_pSoundAsset)
	{
		m_uSFSound->setBuffer(m_pSoundAsset->m_sfSoundBuffer);
		ApplyParams();
		m_uSFSound->play();
	}
}

void SoundPlayer::Stop()
{
	if (m_pSoundAsset)
	{
		m_uSFSound->stop();
	}
}

void SoundPlayer::Pause()
{
	if (m_pSoundAsset && m_status == AudioPlayer::Status::Playing)
	{
		m_uSFSound->pause();
		m_status = AudioPlayer::Status::Paused;
	}
}

void SoundPlayer::Resume()
{
	if (m_pSoundAsset)
	{
		if (m_status == AudioPlayer::Status::Paused)
		{
			m_uSFSound->play();
		}
	}
}

void SoundPlayer::Reset(Time time)
{
	if (m_pSoundAsset)
	{
		m_uSFSound->setPlayingOffset(sf::milliseconds(time.AsMilliseconds()));
	}
}

bool SoundPlayer::IsPlaying() const
{
	return m_pSoundAsset && m_uSFSound->getStatus() == sf::SoundSource::Status::Playing;
}

void SoundPlayer::Tick(Time /*dt*/)
{
	m_status = m_pSoundAsset ? Cast(m_uSFSound->getStatus()) : Status::NoMedia;
	ApplyParams();
}

bool SoundPlayer::ApplyParams()
{
	if (m_pSoundAsset)
	{
		m_uSFSound->setVolume(MixVolume(Mix::SFX, m_volume * m_pSoundAsset->m_volumeScale).ToF32() * 100);
		m_uSFSound->setLoop(m_bLooping);
		return true;
	}
	return false;
}

MusicPlayer::MusicPlayer()
{
	m_uSFMusic = MakeUnique<sf::Music>();
}

MusicPlayer::~MusicPlayer()
{
	if (IsPlaying())
	{
		m_uSFMusic->stop();
	}
}

bool MusicPlayer::SetTrack(std::string path)
{
	return m_uSFMusic->openFromFile(path);
}

Time MusicPlayer::Duration() const
{
	return Time::Microseconds(m_uSFMusic->getDuration().asMicroseconds());
}

bool MusicPlayer::IsFading() const
{
	return m_bFadingIn || m_bFadingOut;
}

void MusicPlayer::FadeIn(Time time, Fixed targetVolume)
{
	m_bFadingOut = false;
	m_bFadingIn = true;
	m_fadeTime = time;
	this->m_targetVolume = Maths::Clamp01(targetVolume);
	BeginFade();
}

void MusicPlayer::FadeOut(Time time, Fixed targetVolume)
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
		if (m_bPauseOnFadeOut)
		{
			m_bPauseOnFadeOut = false;
			Pause();
		}
		else
		{
			Stop();
		}
	}
	m_bFadingIn = m_bFadingOut = false;
}

bool MusicPlayer::IsPaused() const
{
	return m_status == AudioPlayer::Status::Paused;
}

void MusicPlayer::Play()
{
	ApplyParams();
	m_uSFMusic->play();
}

void MusicPlayer::Stop()
{
	m_uSFMusic->stop();
}

void MusicPlayer::Pause()
{
	m_uSFMusic->pause();
	m_status = AudioPlayer::Status::Paused;
}

void MusicPlayer::Resume()
{
	if (m_status == AudioPlayer::Status::Paused)
	{
		m_uSFMusic->play();
	}
}

void MusicPlayer::Reset(Time time)
{
	m_uSFMusic->setPlayingOffset(sf::milliseconds(time.AsMilliseconds()));
}

bool MusicPlayer::IsPlaying() const
{
	return m_uSFMusic->getStatus() == sf::SoundSource::Status::Playing;
}

void MusicPlayer::Tick(Time dt)
{
	m_status = Cast(m_uSFMusic->getStatus());

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
			Fixed ratio = Maths::Clamp01(Fixed(m_elapsedTime.AsMilliseconds()) / Fixed(m_fadeTime.AsMilliseconds()));
			if (m_bFadingIn)
			{
				m_volume = (m_targetVolume - m_startVolume) * ratio;
			}
			else
			{
				m_volume = m_startVolume * (Fixed::One - ratio);
			}
			LOG_H("Fading! Volume: %.2f", m_volume.ToF32());
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
	m_uSFMusic->setVolume(MixVolume(Mix::Music, m_volume).ToF32() * 100);
	m_uSFMusic->setLoop(m_bLooping);
	return true;
}
} // namespace LittleEngine
