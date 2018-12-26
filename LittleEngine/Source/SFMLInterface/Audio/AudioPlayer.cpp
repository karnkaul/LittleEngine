#include "le_stdafx.h"
#include "AudioPlayer.h"
#include "Utils.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "openal32.lib")
#pragma comment(lib, "flac.lib")
#pragma comment(lib, "vorbisenc.lib")
#pragma comment(lib, "vorbisfile.lib")
#pragma comment(lib, "vorbis.lib")
#pragma comment(lib, "ogg.lib")
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "sfml-audio-s-d.lib")
#pragma comment(lib, "sfml-system-s-d.lib")
#else
#pragma comment(lib, "sfml-audio-s.lib")
#pragma comment(lib, "sfml-system-s.lib")
#endif

namespace LittleEngine {
	AudioPlayer::~AudioPlayer() {
		Logger::Log(*this, m_name + " destroyed", Logger::Severity::Debug);
	}

	AudioPlayer::AudioPlayer(const std::string & name) : Object(name) {
		Logger::Log(*this, name + " created", Logger::Severity::Debug);
	}

	AudioPlayer::Status AudioPlayer::Cast(sf::Sound::Status status) {
		switch (status) {
		case sf::Sound::Status::Paused:
			return Status::Paused;
		case sf::Sound::Status::Playing:
			return Status::Playing;
		default:
		case sf::Sound::Status::Stopped:
			return Status::Stopped;
		}
	}

	SoundPlayer::SoundPlayer(SoundAsset* pSoundAsset) : AudioPlayer("SFXPlayer") {
		if (pSoundAsset) SetSoundAsset(*pSoundAsset);
	}

	SoundPlayer::~SoundPlayer() {
		if (IsPlaying()) {
			m_pSoundAsset->m_sfSound.stop();
		}
	}

	bool SoundPlayer::SetSoundAsset(SoundAsset& soundAsset) {
		this->m_pSoundAsset = &soundAsset;
		return true;
	}

	void SoundPlayer::SetDirection(const Fixed& direction) {
		m_pSoundAsset->m_sfSound.setPosition(Maths::Clamp_11(-direction).ToFloat(), 0, 0);
	}

	void SoundPlayer::Play() {
		if (m_pSoundAsset) {
			m_pSoundAsset->m_sfSound.setBuffer(m_pSoundAsset->m_sfSoundBuffer);
			ApplyParams();
			m_pSoundAsset->m_sfSound.play();
		}
	}

	void SoundPlayer::Stop() {
		if (m_pSoundAsset) {
			m_pSoundAsset->m_sfSound.stop();
		}
	}

	void SoundPlayer::Reset(const Fixed& seconds) {
		if (m_pSoundAsset) {
			m_pSoundAsset->m_sfSound.setPlayingOffset(sf::milliseconds(static_cast<sf::Int32>(seconds.ToDouble() * 1000)));
		}
	}

	bool SoundPlayer::IsPlaying() const {
		return m_pSoundAsset && m_pSoundAsset->m_sfSound.getStatus() == sf::SoundSource::Status::Playing;
	}

	void SoundPlayer::Tick(const Fixed&) {
		m_status = m_pSoundAsset ? Cast(m_pSoundAsset->m_sfSound.getStatus()) : Status::NoMedia;
		ApplyParams();
	}

	bool SoundPlayer::ApplyParams() {
		if (m_pSoundAsset) {
			m_pSoundAsset->m_sfSound.setVolume(Maths::Clamp01(m_volume * m_pSoundAsset->m_volumeScale).ToFloat() * 100);
			m_pSoundAsset->m_sfSound.setLoop(m_bLooping);
			return true;
		}
		return false;
	}

	MusicPlayer::MusicPlayer(MusicAsset* pMusicAsset) : AudioPlayer("MusicPlayer") {
		if (pMusicAsset) {
			SetTrack(*pMusicAsset);
		}
	}

	MusicPlayer::~MusicPlayer() {
		if (IsPlaying()) {
			m_pMainTrack->m_sfMusic.stop();
		}
	}

	bool MusicPlayer::SetTrack(MusicAsset& musicAsset) {
		this->m_pMainTrack = &musicAsset;
		return true;
	}

	Fixed MusicPlayer::GetDurationSeconds() const {
		if (m_pMainTrack && m_pMainTrack->m_bValid) {
			if (!m_bLooping) {
				return m_pMainTrack->GetDurationSeconds();
			}
			else {
				return -Fixed::One;
			}
		}
		return Fixed::Zero;
	}

	Fixed MusicPlayer::GetElapsedSeconds() const {
		if (IsPlaying()) {
			return Fixed(m_clock.GetElapsedMilliSeconds(), 1000);
		}
		return Fixed::Zero;
	}

	bool MusicPlayer::IsFading() const {
		return m_bFadingIn || m_bFadingOut;
	}

	void MusicPlayer::FadeIn(const Fixed & timeSeconds, const Fixed & targetVolume) {
		m_bFadingOut = false;
		m_bFadingIn = true;
		m_fadeSeconds = timeSeconds;
		this->m_targetVolume = Maths::Clamp01(targetVolume);
		BeginFade();
	}

	void MusicPlayer::FadeOut(const Fixed & timeSeconds, const Fixed & targetVolume) {
		m_bFadingIn = false;
		m_bFadingOut = true;
		m_fadeSeconds = timeSeconds;
		this->m_targetVolume = Maths::Clamp01(targetVolume);
		BeginFade();
	}

	void MusicPlayer::EndFade() {
		m_volume = m_targetVolume;
		if (m_bFadingOut) {
			Stop();
			m_pMainTrack = nullptr;
		}
		m_bFadingIn = m_bFadingOut = false;
	}

	void MusicPlayer::Play() {
		if (m_pMainTrack && m_pMainTrack->m_bValid) {
			m_clock.Restart();
			ApplyParams();
			m_pMainTrack->m_sfMusic.play();
		}
	}

	void MusicPlayer::Stop() {
		if (m_pMainTrack) {
			m_pMainTrack->m_sfMusic.stop();
		}
	}

	void MusicPlayer::Reset(const Fixed& seconds) {
		if (m_pMainTrack) {
			m_clock.Restart();
			m_pMainTrack->m_sfMusic.setPlayingOffset(sf::milliseconds(static_cast<sf::Int32>(seconds.ToDouble() * 1000)));
		}
	}

	bool MusicPlayer::IsPlaying() const {
		return m_pMainTrack &&  m_pMainTrack->m_sfMusic.getStatus() == sf::SoundSource::Status::Playing;
	}

	void MusicPlayer::Tick(const Fixed& deltaSeconds) {
		m_status = m_pMainTrack ? Cast(m_pMainTrack->m_sfMusic.getStatus()) : Status::NoMedia;
		
		// Process Fade
		if (IsFading()) {
			this->m_elapsedSeconds += deltaSeconds;

			if (this->m_elapsedSeconds >= m_fadeSeconds) {
				EndFade();
			}

			else {
				Fixed ratio = Maths::Clamp01(m_elapsedSeconds / m_fadeSeconds);
				if (m_bFadingIn) {
					m_volume = (m_targetVolume - m_startVolume) * ratio;
				}
				else {
					m_volume = m_startVolume * (Fixed::One - ratio);
				}
				Logger::Log(*this, "Fading! Volume: " + m_volume.ToString(), Logger::Severity::HOT);
			}
		}
		ApplyParams();
	}

	void MusicPlayer::BeginFade() {
		if (!IsPlaying()) {
			m_volume = Fixed(1, 100);
			Play();
		}
		m_startVolume = m_volume;
		m_elapsedSeconds = Fixed::Zero;
	}

	bool MusicPlayer::ApplyParams() {
		if (m_pMainTrack) {
			m_pMainTrack->m_sfMusic.setVolume(Maths::Clamp01(m_volume * m_pMainTrack->m_volumeScale).ToFloat() * 100);
			m_pMainTrack->m_sfMusic.setLoop(m_bLooping);
			return true;
		}
		return false;
	}
}
