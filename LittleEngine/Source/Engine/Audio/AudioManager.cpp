#include "stdafx.h"
#include "AudioManager.h"
#include "Engine/Engine.h"
#include "Utils/Utils.h"

namespace LittleEngine {
	AudioManager::AudioManager(Engine& engine) : Object("AudioManager"), musicPlayer(nullptr) {
		this->engine = &engine;
		Logger::Log(*this, GetNameInBrackets() + " created");
	}

	AudioManager::~AudioManager() {
		Clear();
		Logger::Log(*this, GetNameInBrackets() + " destroyed");
	}

	bool AudioManager::PlaySFX(const std::string & path, const Fixed& volume, bool loop) {
		SoundPlayer& sfxPlayer = GetOrCreateSFXPlayer();
		bool valid = sfxPlayer.SetSoundAsset(GetAssetManager().Load<SoundAsset>(path));
		if (valid) {
			sfxPlayer.volume = volume;
			sfxPlayer.looping = loop;
			sfxPlayer.Play();
		}
		return valid;
	}

	bool AudioManager::IsSFXPlaying() const {
		for (const auto& sfxPlayer : sfxPlayers) {
			if (sfxPlayer->IsPlaying()) {
				return true;
			}
		}
		return false;
	}

	bool AudioManager::PlayMusic(const std::string & path, const Fixed& volume, bool loop) {
		bool valid = musicPlayer.SetMusicAsset(GetAssetManager().Load<MusicAsset>(path));
		if (valid) {
			musicPlayer.volume = volume;
			musicPlayer.looping = loop;
			musicPlayer.Play();
		}
		return valid;
	}

	bool AudioManager::IsMusicPlaying() const {
		return musicPlayer.IsPlaying();
	}

	void AudioManager::StopMusic() {
		// TODO: Fader
		musicPlayer.volume = Fixed::Zero;
		musicPlayer.Stop();
	}

	void AudioManager::Clear() {
		int sfxCount = sfxPlayers.size();
		for (auto& sfxPlayer : sfxPlayers) {
			sfxPlayer->Stop();
		}
		sfxPlayers.clear();
		musicPlayer.Stop();
		Logger::Log(*this, GetNameInBrackets() + " cleared [" + std::to_string(sfxCount) + " SFXPlayers, 1 MusicPlayer]");
	}

	void AudioManager::Tick(Fixed deltaSeconds) {
		// TODO: Fader
		musicPlayer.Tick(deltaSeconds);
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
}
