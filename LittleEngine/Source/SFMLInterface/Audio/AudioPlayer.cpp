#include "stdafx.h"
#include "AudioPlayer.h"
#include "Utils/Utils.h"

namespace LittleEngine {
	AudioPlayer::~AudioPlayer() {
		Logger::Log(*this, name + " destroyed", Logger::Severity::Debug);
	}

	AudioPlayer::AudioPlayer(const std::string & name) : Object(name) {
		Logger::Log(*this, name + " created", Logger::Severity::Debug);
	}

	SoundPlayer::SoundPlayer(SoundAsset::Ptr soundAsset) : AudioPlayer("SFXPlayer") {
		SetSoundAsset(soundAsset);
	}

	SoundPlayer::~SoundPlayer() {
		if (IsPlaying()) {
			soundAsset->sfSound.stop();
		}
	}

	bool SoundPlayer::SetSoundAsset(SoundAsset::Ptr soundAsset) {
		if (soundAsset) {
			this->soundAsset = soundAsset;
			return true;
		}
		return false;
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
			soundAsset->sfSound.setPlayingOffset(sf::milliseconds(static_cast<sf::Int32>(seconds.GetDouble() * 1000)));
		}
	}

	bool SoundPlayer::IsPlaying() const {
		return soundAsset && soundAsset->sfSound.getStatus() == sf::SoundSource::Status::Playing;
	}

	void SoundPlayer::Tick(Fixed deltaSeconds) {
		ApplyParams();
	}

	bool SoundPlayer::ApplyParams() {
		if (soundAsset) {
			soundAsset->sfSound.setVolume(Maths::Clamp01(volume * soundAsset->volumeScale).GetFloat() * 100);
			soundAsset->sfSound.setLoop(looping);
			return true;
		}
		return false;
	}

	MusicPlayer::MusicPlayer(MusicAsset::Ptr musicAsset) : AudioPlayer("MusicPlayer") {
		SetTrack(musicAsset);
	}

	MusicPlayer::~MusicPlayer() {
		if (IsPlaying()) {
			mainTrack->music.stop();
		}
	}

	bool MusicPlayer::SetTrack(MusicAsset::Ptr musicAsset) {
		if (musicAsset) {
			this->mainTrack = musicAsset;
			return true;
		}
		return false;
	}

	Fixed MusicPlayer::GetDurationSeconds() const {
		if (mainTrack && mainTrack->valid) {
			if (!looping) {
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
		return fadingIn || fadingOut;
	}

	void MusicPlayer::FadeIn(const Fixed & timeSeconds, const Fixed & targetVolume) {
		fadingOut = false;
		fadingIn = true;
		fadeSeconds = timeSeconds;
		this->targetVolume = Maths::Clamp01(targetVolume);
		BeginFade();
	}

	void MusicPlayer::FadeOut(const Fixed & timeSeconds, const Fixed & targetVolume) {
		fadingIn = false;
		fadingOut = true;
		fadeSeconds = timeSeconds;
		this->targetVolume = Maths::Clamp01(targetVolume);
		BeginFade();
	}

	void MusicPlayer::EndFade() {
		volume = targetVolume;
		if (fadingOut) {
			Stop();
			mainTrack = nullptr;
		}
		fadingIn = fadingOut = false;
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
			mainTrack->music.setPlayingOffset(sf::milliseconds(static_cast<sf::Int32>(seconds.GetDouble() * 1000)));
		}
	}

	bool MusicPlayer::IsPlaying() const {
		return mainTrack &&  mainTrack->music.getStatus() == sf::SoundSource::Status::Playing;
	}

	void MusicPlayer::Tick(Fixed deltaSeconds) {
		// Process Fade
		if (IsFading()) {
			this->elapsedSeconds += deltaSeconds;

			if (this->elapsedSeconds >= fadeSeconds) {
				EndFade();
			}

			else {
				Fixed ratio = Maths::Clamp01(elapsedSeconds / fadeSeconds);
				if (fadingIn) {
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
			mainTrack->music.setVolume(Maths::Clamp01(volume * mainTrack->volumeScale).GetFloat() * 100);
			mainTrack->music.setLoop(looping);
			return true;
		}
		return false;
	}
}
