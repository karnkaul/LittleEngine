#include "stdafx.h"
#include "AudioManager.h"
#include "Engine/Engine.h"
#include "Utils/Utils.h"

namespace LittleEngine {
	AudioManager::AudioManager(Engine& engine) : Object("AudioManager"), musicPlayer(nullptr), musicFader(musicPlayer) {
		this->engine = &engine;
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
			musicPlayer.looping = loop;
			if (musicPlayer.IsPlaying()) {
				musicPlayer.Stop();
			}
			musicPlayer.SetTrack(asset);
			if (fadeSeconds > Fixed::Zero) {
				musicFader.FadeIn(fadeSeconds, volume);
			}
			else {
				musicPlayer.volume = volume;
				musicPlayer.Play();
			}
		}
		return valid;
	}

	bool AudioManager::IsMusicPlaying() const {
		return musicPlayer.IsPlaying();
	}

	void AudioManager::StopMusic(const Fixed& fadeSeconds) {
		musicFader.FadeOut(fadeSeconds, Fixed::Zero);
	}

	void AudioManager::SwitchTrack(const std::string& path, const Fixed& volume, const Fixed & fadeSeconds) {
		MusicAsset::Ptr newTrack = GetAssetManager().Load<MusicAsset>(path);
		if (newTrack != nullptr) {
			switchTrackRequest = std::make_unique<SwitchTrackRequest>(newTrack, fadeSeconds, volume);
			if (IsMusicPlaying()) {
				musicFader.FadeOut(fadeSeconds * Fixed::Half);
				switchTrackRequest->fadingOldTrack = true;
				switchTrackRequest->fadeSeconds *= Fixed::Half;
			}
		}
	}

	void AudioManager::SetMusicVolume(const Fixed & volume) {
		if (switchTrackRequest != nullptr) {
			switchTrackRequest->targetVolume = volume;
		}
		else if (musicFader.IsFading()) {
			musicFader.targetVolume = volume;
		}
		else {
			musicPlayer.volume = volume;
		}
	}

	void AudioManager::Clear(bool immediate) {
		int sfxCount = sfxPlayers.size();
		for (auto& sfxPlayer : sfxPlayers) {
			sfxPlayer->Stop();
		}
		sfxPlayers.clear();
		if (immediate) {
			musicFader.EndFade();
			musicPlayer.Stop();
		}
		else if (musicPlayer.IsPlaying()) {
			musicFader.FadeOut(Fixed::Half);
		}
		switchTrackRequest = nullptr;
		Logger::Log(*this, GetNameInBrackets() + " cleared [" + std::to_string(sfxCount) + " SFXPlayers, 1 MusicPlayer]");
	}

	void AudioManager::Tick(Fixed deltaTime) {
		if (switchTrackRequest != nullptr && !musicFader.IsFading()) {
			if (musicPlayer.IsPlaying()) {
				musicPlayer.Stop();
			}
			musicPlayer.SetTrack(switchTrackRequest->newTrack);
			musicFader.FadeIn(switchTrackRequest->fadeSeconds, switchTrackRequest->targetVolume);
			switchTrackRequest = nullptr;
		}
		musicPlayer.Tick(deltaTime);
		musicFader.Tick(deltaTime);
		for (auto& sfxPlayer : sfxPlayers) {
			sfxPlayer->Tick(deltaTime);
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

	AudioManager::MusicFader::MusicFader(MusicPlayer & player) {
		this->player = &player;
	}

	bool AudioManager::MusicFader::IsFading() const {
		return fadingIn || fadingOut;
	}

	void AudioManager::MusicFader::FadeIn(const Fixed & timeSeconds, const Fixed & targetVolume) {
		fadingOut = false;
		fadingIn = true;
		fadeSeconds = timeSeconds;
		this->targetVolume = Maths::Clamp01(targetVolume);
		BeginFade();
	}

	void AudioManager::MusicFader::FadeOut(const Fixed & timeSeconds, const Fixed & targetVolume) {
		fadingIn = false;
		fadingOut = true;
		fadeSeconds = timeSeconds;
		this->targetVolume = Maths::Clamp01(targetVolume);
		BeginFade();
	}

	void AudioManager::MusicFader::Tick(const Fixed & deltaTime) {
		if (IsFading()) {
			this->elapsedSeconds += (deltaTime / 1000);

			if (this->elapsedSeconds >= fadeSeconds) {
				EndFade();
				return;
			}

			Fixed ratio = Maths::Clamp01(elapsedSeconds / fadeSeconds);
			if (fadingIn) {
				player->volume = (targetVolume - startVolume) * ratio;
			}
			else {
				player->volume = startVolume * (Fixed::One - ratio);
			}
		}
	}

	void AudioManager::MusicFader::BeginFade() {
		if (!player->IsPlaying()) {
			player->volume = Fixed(1, 100);
			player->Play();
		}
		startVolume = player->volume;
		elapsedSeconds = Fixed::Zero;
	}

	void AudioManager::MusicFader::EndFade() {
		player->volume = targetVolume;
		if (fadingOut) {
			player->Stop();
		}
		fadingIn = fadingOut = false;
	}
	
	AudioManager::SwitchTrackRequest::SwitchTrackRequest(MusicAsset::Ptr newTrack, const Fixed & fadeSeconds, const Fixed& targetVolume) {
		this->fadeSeconds = fadeSeconds;
		this->newTrack = newTrack;
		this->targetVolume = targetVolume;
	}
}
