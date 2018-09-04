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
			ApplyParams(false);
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
		ApplyParams(false);
	}

	bool SoundPlayer::ApplyParams(bool replaceSound) {
		if (soundAsset) {
			soundAsset->sfSound.setVolume(Maths::Clamp01(volume * soundAsset->volumeScale).GetFloat() * 100);
			soundAsset->sfSound.setLoop(looping);
			if (replaceSound) {
				soundAsset->sfSound.setBuffer(soundAsset->sfSoundBuffer);
				return false;
			}
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

	void MusicPlayer::Play() {
		if (mainTrack && mainTrack->valid) {
			clock.Restart();
			ApplyParams(false);
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
		ApplyParams(false);
	}

	bool MusicPlayer::ApplyParams(bool replaceMusic) {
		if (mainTrack) {
			mainTrack->music.setVolume(Maths::Clamp01(volume * mainTrack->volumeScale).GetFloat() * 100);
			mainTrack->music.setLoop(looping);
			if (replaceMusic) {
				return mainTrack->music.openFromFile(mainTrack->resourcePath);
			}
			return true;
		}
		return false;
	}
}
