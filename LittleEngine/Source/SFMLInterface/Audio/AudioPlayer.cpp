#include "le_stdafx.h"
#include "AudioPlayer.h"
#include "Utils.h"

namespace LittleEngine {
	AudioPlayer::~AudioPlayer() {
		Logger::Log(*this, name + " destroyed", Logger::Severity::Debug);
	}

	AudioPlayer::AudioPlayer(const std::string & name) : Object(name) {
		Logger::Log(*this, name + " created", Logger::Severity::Debug);
	}

	AudioPlayer::Status AudioPlayer::Convert(sf::Sound::Status status) {
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

	SoundPlayer::SoundPlayer(SoundAsset* soundAsset) : AudioPlayer("SFXPlayer") {
		if (soundAsset) SetSoundAsset(*soundAsset);
	}

	SoundPlayer::~SoundPlayer() {
		if (IsPlaying()) {
			soundAsset->sfSound.stop();
		}
	}

	bool SoundPlayer::SetSoundAsset(SoundAsset& soundAsset) {
		this->soundAsset = &soundAsset;
		return true;
	}

	void SoundPlayer::SetDirection(const Fixed& direction) {
		soundAsset->sfSound.setPosition(Maths::Clamp_11(-direction).ToFloat(), 0, 0);
	}

	void SoundPlayer::Play() {
		if (soundAsset) {
			soundAsset->sfSound.setBuffer(soundAsset->sfSoundBuffer);
			ApplyParams();
			soundAsset->sfSound.play();
		}
	}

	void SoundPlayer::Stop() {
		if (soundAsset) {
			soundAsset->sfSound.stop();
		}
	}

	void SoundPlayer::Reset(Fixed seconds) {
		if (soundAsset) {
			soundAsset->sfSound.setPlayingOffset(sf::milliseconds(static_cast<sf::Int32>(seconds.ToDouble() * 1000)));
		}
	}

	bool SoundPlayer::IsPlaying() const {
		return soundAsset && soundAsset->sfSound.getStatus() == sf::SoundSource::Status::Playing;
	}

	void SoundPlayer::Tick(Fixed deltaSeconds) {
		status = soundAsset ? Convert(soundAsset->sfSound.getStatus()) : Status::NoMedia;
		ApplyParams();
	}

	bool SoundPlayer::ApplyParams() {
		if (soundAsset) {
			soundAsset->sfSound.setVolume(Maths::Clamp01(volume * soundAsset->volumeScale).ToFloat() * 100);
			soundAsset->sfSound.setLoop(bLooping);
			return true;
		}
		return false;
	}

	MusicPlayer::MusicPlayer(MusicAsset* musicAsset) : AudioPlayer("MusicPlayer") {
		if (musicAsset) {
			SetTrack(*musicAsset);
		}
	}

	MusicPlayer::~MusicPlayer() {
		if (IsPlaying()) {
			mainTrack->music.stop();
		}
	}

	bool MusicPlayer::SetTrack(MusicAsset& musicAsset) {
		this->mainTrack = &musicAsset;
		return true;
	}

	Fixed MusicPlayer::GetDurationSeconds() const {
		if (mainTrack && mainTrack->valid) {
			if (!bLooping) {
				return mainTrack->GetDurationSeconds();
			}
			else {
				return -Fixed::One;
			}
		}
		return Fixed::Zero;
	}

	Fixed MusicPlayer::GetElapsedSeconds() const {
		if (IsPlaying()) {
			return Fixed(clock.GetElapsedMilliSeconds(), 1000);
		}
		return Fixed::Zero;
	}

	bool MusicPlayer::IsFading() const {
		return bFadingIn || bFadingOut;
	}

	void MusicPlayer::FadeIn(const Fixed & timeSeconds, const Fixed & targetVolume) {
		bFadingOut = false;
		bFadingIn = true;
		fadeSeconds = timeSeconds;
		this->targetVolume = Maths::Clamp01(targetVolume);
		BeginFade();
	}

	void MusicPlayer::FadeOut(const Fixed & timeSeconds, const Fixed & targetVolume) {
		bFadingIn = false;
		bFadingOut = true;
		fadeSeconds = timeSeconds;
		this->targetVolume = Maths::Clamp01(targetVolume);
		BeginFade();
	}

	void MusicPlayer::EndFade() {
		volume = targetVolume;
		if (bFadingOut) {
			Stop();
			mainTrack = nullptr;
		}
		bFadingIn = bFadingOut = false;
	}

	void MusicPlayer::Play() {
		if (mainTrack && mainTrack->valid) {
			clock.Restart();
			ApplyParams();
			mainTrack->music.play();
		}
	}

	void MusicPlayer::Stop() {
		if (mainTrack) {
			mainTrack->music.stop();
		}
	}

	void MusicPlayer::Reset(Fixed seconds) {
		if (mainTrack) {
			clock.Restart();
			mainTrack->music.setPlayingOffset(sf::milliseconds(static_cast<sf::Int32>(seconds.ToDouble() * 1000)));
		}
	}

	bool MusicPlayer::IsPlaying() const {
		return mainTrack &&  mainTrack->music.getStatus() == sf::SoundSource::Status::Playing;
	}

	void MusicPlayer::Tick(Fixed deltaSeconds) {
		status = mainTrack ? Convert(mainTrack->music.getStatus()) : Status::NoMedia;
		
		// Process Fade
		if (IsFading()) {
			this->elapsedSeconds += deltaSeconds;

			if (this->elapsedSeconds >= fadeSeconds) {
				EndFade();
			}

			else {
				Fixed ratio = Maths::Clamp01(elapsedSeconds / fadeSeconds);
				if (bFadingIn) {
					volume = (targetVolume - startVolume) * ratio;
				}
				else {
					volume = startVolume * (Fixed::One - ratio);
				}
				Logger::Log(*this, "Fading! Volume: " + volume.ToString(), Logger::Severity::Debug);
			}
		}
		ApplyParams();
	}

	void MusicPlayer::BeginFade() {
		if (!IsPlaying()) {
			volume = Fixed(1, 100);
			Play();
		}
		startVolume = volume;
		elapsedSeconds = Fixed::Zero;
	}

	bool MusicPlayer::ApplyParams() {
		if (mainTrack) {
			mainTrack->music.setVolume(Maths::Clamp01(volume * mainTrack->volumeScale).ToFloat() * 100);
			mainTrack->music.setLoop(bLooping);
			return true;
		}
		return false;
	}
}
