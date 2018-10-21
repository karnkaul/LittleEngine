#include "le_stdafx.h"
#include "AudioManager.h"
#include "Engine/Engine.h"
#include "Utils.h"

namespace LittleEngine {
	AudioManager::AudioManager(Engine& engine) : Object("AudioManager"), engine(&engine) {
		Logger::Log(*this, GetNameInBrackets() + " created");
	}

	AudioManager::~AudioManager() {
		Clear();
		Logger::Log(*this, GetNameInBrackets() + " destroyed");
	}

	SoundPlayer* AudioManager::PlaySFX(const std::string & path, const Fixed& volume, bool loop) {
		SoundPlayer& sfxPlayer = GetOrCreateSFXPlayer();
		bool valid = sfxPlayer.SetSoundAsset(GetAssetManager().Load<SoundAsset>(path));
		if (valid) {
			sfxPlayer.volume = volume;
			sfxPlayer.looping = loop;
			sfxPlayer.Play();
			return &sfxPlayer;
		}
		return nullptr;
	}

	bool AudioManager::IsSFXPlaying() const {
		for (const auto& sfxPlayer : sfxPlayers) {
			if (sfxPlayer->IsPlaying()) {
				return true;
			}
		}
		return false;
	}

	bool AudioManager::PlayMusic(const std::string & path, const Fixed& volume, const Fixed& fadeSeconds, bool loop) {
		MusicAsset::Ptr asset = GetAssetManager().Load<MusicAsset>(path);
		bool valid = asset->valid;
		if (valid) {
			switchTrackRequest = nullptr;
			MusicPlayer& active = GetActivePlayer();
			active.looping = loop;
			if (active.IsPlaying()) {
				active.Stop();
			}
			active.SetTrack(asset);
			if (fadeSeconds > Fixed::Zero) {
				active.FadeIn(fadeSeconds, volume);
			}
			else {
				active.volume = volume;
				active.Play();
			}
		}
		return valid;
	}

	bool AudioManager::IsMusicPlaying() const {
		return GetActivePlayer().IsPlaying();
	}

	void AudioManager::StopMusic(const Fixed& fadeSeconds) {
		GetActivePlayer().FadeOut(fadeSeconds, Fixed::Zero);
		GetStandbyPlayer().Stop();
	}

	void AudioManager::SwitchTrack(const std::string& path, const Fixed& volume, const Fixed & fadeSeconds) {
		MusicAsset::Ptr newTrack = GetAssetManager().Load<MusicAsset>(path);
		if (newTrack != nullptr) {
			switchTrackRequest = std::make_unique<SwitchTrackRequest>(newTrack, fadeSeconds, volume);
			if (IsMusicPlaying()) {
				GetActivePlayer().FadeOut(fadeSeconds * Fixed::OneHalf);
				switchTrackRequest->fadingOldTrack = true;
				switchTrackRequest->fadeSeconds *= Fixed::OneHalf;
			}
		}
	}

	void AudioManager::SetMusicVolume(const Fixed & volume) {
		if (switchTrackRequest != nullptr) {
			switchTrackRequest->targetVolume = volume;
		}
		else if (musicPlayerA.IsFading()) {
			musicPlayerA.targetVolume = volume;
		}
		else {
			musicPlayerA.volume = volume;
		}
	}

	void AudioManager::Clear(bool immediate) {
		int sfxCount = static_cast<int>(sfxPlayers.size());
		for (auto& sfxPlayer : sfxPlayers) {
			sfxPlayer->Stop();
		}
		sfxPlayers.clear();
		MusicPlayer& active = GetActivePlayer();
		if (immediate) {
			active.EndFade();
			active.Stop();
		}
		else if (active.IsPlaying()) {
			active.FadeOut(Fixed::OneHalf);
		}
		GetStandbyPlayer().Stop();
		switchTrackRequest = nullptr;
		Logger::Log(*this, GetNameInBrackets() + " cleared [" + std::to_string(sfxCount) + " SFXPlayers, 1 MusicPlayer]");
	}

	void AudioManager::Tick(Fixed deltaTime) {
		MusicPlayer& active = GetActivePlayer();
		if (switchTrackRequest != nullptr && !active.IsFading()) {
			if (active.IsPlaying()) {
				active.Stop();
			}
			active.SetTrack(switchTrackRequest->newTrack);
			active.FadeIn(switchTrackRequest->fadeSeconds, switchTrackRequest->targetVolume);
			switchTrackRequest = nullptr;
		}
		Fixed deltaSeconds = deltaTime * Fixed(1, 1000);
		active.Tick(deltaSeconds);
		GetStandbyPlayer().Tick(deltaSeconds);
		for (auto& sfxPlayer : sfxPlayers) {
			sfxPlayer->Tick(deltaSeconds);
		}
	}

	AssetManager & AudioManager::GetAssetManager() {
		return engine->GetAssetManager();
	}

	SoundPlayer& AudioManager::GetOrCreateSFXPlayer() {
		for (auto& sfxPlayer : sfxPlayers) {
			if (!sfxPlayer->IsPlaying()) {
				return *sfxPlayer;
			}
		}
		sfxPlayers.push_back(std::make_unique<SoundPlayer>(nullptr));
		return *sfxPlayers[sfxPlayers.size() - 1];
	}

	MusicPlayer & AudioManager::GetActivePlayer() {
		return sideA ? musicPlayerA : musicPlayerB;
	}

	MusicPlayer & AudioManager::GetStandbyPlayer() {
		return sideA ? musicPlayerB : musicPlayerA;
	}

	const MusicPlayer & AudioManager::GetActivePlayer() const {
		return sideA ? musicPlayerA : musicPlayerB;
	}

	const MusicPlayer & AudioManager::GetStandbyPlayer() const {
		return sideA ? musicPlayerB : musicPlayerA;
	}
	
	AudioManager::SwitchTrackRequest::SwitchTrackRequest(MusicAsset::Ptr newTrack, const Fixed & fadeSeconds, const Fixed& targetVolume) {
		this->fadeSeconds = fadeSeconds;
		this->newTrack = newTrack;
		this->targetVolume = targetVolume;
	}
}
