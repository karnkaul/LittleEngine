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
		SetMusicAsset(musicAsset);
	}

	MusicPlayer::~MusicPlayer() {
		if (IsPlaying()) {
			musicAsset->music.stop();
		}
	}

	bool MusicPlayer::SetMusicAsset(MusicAsset::Ptr musicAsset) {
		if (musicAsset) {
			this->musicAsset = musicAsset;
			return true;
		}
		return false;
	}

	Fixed MusicPlayer::GetDurationSeconds() const {
		if (musicAsset && musicAsset->valid) {
			if (!looping) {
				return musicAsset->GetDurationSeconds();
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
		if (musicAsset && musicAsset->valid) {
			clock.Restart();
			ApplyParams(false);
			musicAsset->music.play();
		}
	}

	void MusicPlayer::Stop() {
		if (musicAsset) {
			musicAsset->music.stop();
		}
	}

	void MusicPlayer::Reset(Fixed seconds) {
		if (musicAsset) {
			clock.Restart();
			musicAsset->music.setPlayingOffset(sf::milliseconds(static_cast<sf::Int32>(seconds.GetDouble() * 1000)));
		}
	}

	bool MusicPlayer::IsPlaying() const {
		return musicAsset &&  musicAsset->music.getStatus() == sf::SoundSource::Status::Playing;
	}

	void MusicPlayer::Tick(Fixed deltaSeconds) {
		ApplyParams(false);
	}

	bool MusicPlayer::ApplyParams(bool replaceMusic) {
		if (musicAsset) {
			musicAsset->music.setVolume(Maths::Clamp01(volume * musicAsset->volumeScale).GetFloat() * 100);
			musicAsset->music.setLoop(looping);
			if (replaceMusic) {
				return musicAsset->music.openFromFile(musicAsset->resourcePath);
			}
			return true;
		}
		return false;
	}
}
