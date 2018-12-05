#include "le_stdafx.h"
#include "AudioManager.h"
#include "Engine/Engine.h"
#include "Utils.h"

namespace LittleEngine {
	AudioManager::AudioManager(Engine& engine) : Object("AudioManager"), m_pEngine(&engine) {
		Logger::Log(*this, GetNameInBrackets() + " created");
	}

	AudioManager::~AudioManager() {
		Clear();
		Logger::Log(*this, GetNameInBrackets() + " destroyed");
	}

	SoundPlayer* AudioManager::PlaySFX(const std::string& path, const Fixed& volume, const Fixed& direction, bool bLoop) {
		SoundPlayer& sfxPlayer = GetOrCreateSFXPlayer();
		SoundAsset* asset = GetAssetManager().Load<SoundAsset>(path);
		if (asset) {
			sfxPlayer.SetSoundAsset(*asset);
			sfxPlayer.m_volume = volume;
			sfxPlayer.m_bLooping = bLoop;
			sfxPlayer.SetDirection(direction);
			sfxPlayer.Play();
			return &sfxPlayer;
		}
		return nullptr;
	}

	SoundPlayer * AudioManager::PlaySFX(SoundAsset& sound, const Fixed & volume, const Fixed & direction, bool bLoop) {
		SoundPlayer& sfxPlayer = GetOrCreateSFXPlayer();
		sfxPlayer.SetSoundAsset(sound);
		sfxPlayer.m_volume = volume;
		sfxPlayer.m_bLooping = bLoop;
		sfxPlayer.SetDirection(direction);
		sfxPlayer.Play();
		return &sfxPlayer;
	}

	bool AudioManager::IsSFXPlaying() const {
		for (const auto& sfxPlayer : m_sfxPlayers) {
			if (sfxPlayer->IsPlaying()) {
				return true;
			}
		}
		return false;
	}

	bool AudioManager::PlayMusic(const std::string & path, const Fixed& volume, const Fixed& fadeSeconds, bool bLoop) {
		MusicAsset* asset = GetAssetManager().Load<MusicAsset>(path);
		bool bValid = asset->m_bValid;
		if (bValid) {
			m_uSwitchTrackRequest = nullptr;
			MusicPlayer& active = GetActivePlayer();
			active.m_bLooping = bLoop;
			if (active.IsPlaying()) {
				active.Stop();
			}
			active.SetTrack(*asset);
			if (fadeSeconds > Fixed::Zero) {
				active.FadeIn(fadeSeconds, volume);
			}
			else {
				active.m_volume = volume;
				active.Play();
			}
		}
		return bValid;
	}

	bool AudioManager::IsMusicPlaying() const {
		return GetActivePlayer().IsPlaying();
	}

	void AudioManager::StopMusic(const Fixed& fadeSeconds) {
		GetActivePlayer().FadeOut(fadeSeconds, Fixed::Zero);
		GetStandbyPlayer().Stop();
	}

	void AudioManager::SwitchTrack(const std::string& path, const Fixed& volume, const Fixed & fadeSeconds) {
		MusicAsset* newTrack = GetAssetManager().Load<MusicAsset>(path);
		if (newTrack != nullptr) {
			m_uSwitchTrackRequest = std::make_unique<SwitchTrackRequest>(*newTrack, fadeSeconds, volume);
			if (IsMusicPlaying()) {
				GetActivePlayer().FadeOut(fadeSeconds * Fixed::OneHalf);
				m_uSwitchTrackRequest->bFadingOldTrack = true;
				m_uSwitchTrackRequest->fadeSeconds *= Fixed::OneHalf;
			}
		}
	}

	void AudioManager::SetMusicVolume(const Fixed & volume) {
		if (m_uSwitchTrackRequest != nullptr) {
			m_uSwitchTrackRequest->targetVolume = volume;
		}
		else if (m_musicPlayerA.IsFading()) {
			m_musicPlayerA.m_targetVolume = volume;
		}
		else {
			m_musicPlayerA.m_volume = volume;
		}
	}

	void AudioManager::Clear(bool immediate) {
		int sfxCount = static_cast<int>(m_sfxPlayers.size());
		for (auto& sfxPlayer : m_sfxPlayers) {
			sfxPlayer->Stop();
		}
		m_sfxPlayers.clear();
		MusicPlayer& active = GetActivePlayer();
		if (immediate) {
			active.EndFade();
			active.Stop();
		}
		else if (active.IsPlaying()) {
			active.FadeOut(Fixed::OneHalf);
		}
		GetStandbyPlayer().Stop();
		m_uSwitchTrackRequest = nullptr;
		Logger::Log(*this, GetNameInBrackets() + " cleared [" + Strings::ToString(sfxCount) + " SFXPlayers, 1 MusicPlayer]");
	}

	void AudioManager::Tick(Fixed deltaTime) {
		MusicPlayer& active = GetActivePlayer();
		if (m_uSwitchTrackRequest != nullptr && !active.IsFading()) {
			if (active.IsPlaying()) active.Stop();
			active.SetTrack(*m_uSwitchTrackRequest->newTrack);
			active.FadeIn(m_uSwitchTrackRequest->fadeSeconds, m_uSwitchTrackRequest->targetVolume);
			m_uSwitchTrackRequest = nullptr;
		}
		Fixed deltaSeconds = deltaTime * Fixed(1, 1000);
		active.Tick(deltaSeconds);
		GetStandbyPlayer().Tick(deltaSeconds);
		for (auto& sfxPlayer : m_sfxPlayers) {
			sfxPlayer->Tick(deltaSeconds);
		}
	}

	AssetManager & AudioManager::GetAssetManager() {
		return m_pEngine->GetAssetManager();
	}

	SoundPlayer& AudioManager::GetOrCreateSFXPlayer() {
		for (auto& sfxPlayer : m_sfxPlayers) {
			if (!sfxPlayer->IsPlaying()) {
				return *sfxPlayer;
			}
		}
		m_sfxPlayers.push_back(std::make_unique<SoundPlayer>(nullptr));
		return *m_sfxPlayers[m_sfxPlayers.size() - 1];
	}

	MusicPlayer & AudioManager::GetActivePlayer() {
		return m_bSideA ? m_musicPlayerA : m_musicPlayerB;
	}

	MusicPlayer & AudioManager::GetStandbyPlayer() {
		return m_bSideA ? m_musicPlayerB : m_musicPlayerA;
	}

	const MusicPlayer & AudioManager::GetActivePlayer() const {
		return m_bSideA ? m_musicPlayerA : m_musicPlayerB;
	}

	const MusicPlayer & AudioManager::GetStandbyPlayer() const {
		return m_bSideA ? m_musicPlayerB : m_musicPlayerA;
	}
}
