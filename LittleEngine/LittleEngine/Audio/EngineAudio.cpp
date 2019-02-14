#include "stdafx.h"
#include "EngineAudio.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Repository/EngineRepository.h"
#include "Utils.h"

namespace LittleEngine
{
EngineAudio::EngineAudio()
{
	LogD("[EngineAudio] constructed");
}

EngineAudio::~EngineAudio()
{
	Clear();
	LogD("[EngineAudio] destroyed");
}

SoundPlayer* EngineAudio::PlaySFX(const String& path, const Fixed& volume, const Fixed& direction, bool bLoop)
{
	SoundPlayer& sfxPlayer = GetOrCreateSFXPlayer();
	SoundAsset* asset = GetAssetManager().Load<SoundAsset>(path);
	if (asset)
	{
		sfxPlayer.SetSoundAsset(*asset);
		sfxPlayer.m_volume = volume;
		sfxPlayer.m_bLooping = bLoop;
		sfxPlayer.SetDirection(direction);
		sfxPlayer.Play();
		return &sfxPlayer;
	}
	return nullptr;
}

SoundPlayer* EngineAudio::PlaySFX(SoundAsset& sound, const Fixed& volume, const Fixed& direction, bool bLoop)
{
	SoundPlayer& sfxPlayer = GetOrCreateSFXPlayer();
	sfxPlayer.SetSoundAsset(sound);
	sfxPlayer.m_volume = volume;
	sfxPlayer.m_bLooping = bLoop;
	sfxPlayer.SetDirection(direction);
	sfxPlayer.Play();
	return &sfxPlayer;
}

bool EngineAudio::IsSFXPlaying() const
{
	for (const auto& sfxPlayer : m_sfxPlayers)
	{
		if (sfxPlayer->IsPlaying())
		{
			return true;
		}
	}
	return false;
}

bool EngineAudio::PlayMusic(const String& path, const Fixed& volume, Time fadeTime, bool bLoop)
{
	MusicAsset* asset = GetAssetManager().Load<MusicAsset>(path);
	bool bValid = asset->m_bValid;
	if (bValid)
	{
		m_uSwitchTrackRequest = nullptr;
		MusicPlayer& active = GetActivePlayer();
		active.m_bLooping = bLoop;
		if (active.IsPlaying())
		{
			active.Stop();
		}
		active.SetTrack(*asset);
		if (fadeTime.AsSeconds() > 0)
		{
			active.FadeIn(fadeTime, volume);
		}
		else
		{
			active.m_volume = volume;
			active.Play();
		}
	}
	return bValid;
}

bool EngineAudio::IsMusicPlaying() const
{
	return GetActivePlayer().IsPlaying();
}

void EngineAudio::StopMusic(Time fadeTime)
{
	GetActivePlayer().FadeOut(fadeTime, Fixed::Zero);
	GetStandbyPlayer().Stop();
}

bool EngineAudio::ResumeMusic(Time fadeTime, const Fixed& volume)
{
	MusicPlayer& player = GetActivePlayer();
	if (player.HasTrack())
	{
		if (fadeTime.AsSeconds() > 0.0f)
		{
			player.FadeIn(fadeTime, volume);
		}
		else
		{
			player.m_volume = volume;
			player.Play();
		}
		return true;
	}
	return false;
}

void EngineAudio::SwitchTrack(const String& path, const Fixed& volume, Time fadeTime)
{
	MusicAsset* newTrack = GetAssetManager().Load<MusicAsset>(path);
	if (newTrack != nullptr)
	{
		m_uSwitchTrackRequest = MakeUnique<SwitchTrackRequest>(*newTrack, fadeTime, volume);
		if (IsMusicPlaying())
		{
			GetActivePlayer().FadeOut(fadeTime.Scale(Fixed::OneHalf));
			m_uSwitchTrackRequest->bFadingOldTrack = true;
			m_uSwitchTrackRequest->fadeTime.Scale(Fixed::OneHalf);
		}
	}
}

void EngineAudio::SetMusicVolume(const Fixed& volume)
{
	if (m_uSwitchTrackRequest != nullptr)
	{
		m_uSwitchTrackRequest->targetVolume = volume;
	}
	else if (m_musicPlayerA.IsFading())
	{
		m_musicPlayerA.m_targetVolume = volume;
	}
	else
	{
		m_musicPlayerA.m_volume = volume;
	}
}

void EngineAudio::Clear(bool immediate)
{
#if DEBUGGING
	u32 sfxCount = static_cast<u32>(m_sfxPlayers.size());
#endif
	for (auto& sfxPlayer : m_sfxPlayers)
	{
		sfxPlayer->Stop();
	}
	m_sfxPlayers.clear();
	MusicPlayer& active = GetActivePlayer();
	if (immediate)
	{
		active.EndFade();
		active.Stop();
	}
	else if (active.IsPlaying())
	{
		active.FadeOut(Time::Seconds(0.5f));
	}
	GetStandbyPlayer().Stop();
	m_uSwitchTrackRequest = nullptr;
	LogD("[EngineAudio] Cleared [" + Strings::ToString(sfxCount) + " SFXPlayers, 1 MusicPlayer]");
}

void EngineAudio::Tick(Time dt)
{
	MusicPlayer& active = GetActivePlayer();
	if (m_uSwitchTrackRequest != nullptr && !active.IsFading())
	{
		if (active.IsPlaying())
			active.Stop();
		active.SetTrack(*m_uSwitchTrackRequest->newTrack);
		active.FadeIn(m_uSwitchTrackRequest->fadeTime, m_uSwitchTrackRequest->targetVolume);
		m_uSwitchTrackRequest = nullptr;
	}
	active.Tick(dt);
	GetStandbyPlayer().Tick(dt);
	for (auto& sfxPlayer : m_sfxPlayers)
	{
		sfxPlayer->Tick(dt);
	}
}

EngineRepository& EngineAudio::GetAssetManager()
{
	return *Services::Engine()->Repository();
}

SoundPlayer& EngineAudio::GetOrCreateSFXPlayer()
{
	for (auto& sfxPlayer : m_sfxPlayers)
	{
		if (!sfxPlayer->IsPlaying())
		{
			return *sfxPlayer;
		}
	}
	m_sfxPlayers.push_back(MakeUnique<SoundPlayer>(nullptr));
	return *m_sfxPlayers[m_sfxPlayers.size() - 1];
}

MusicPlayer& EngineAudio::GetActivePlayer()
{
	return m_bSideA ? m_musicPlayerA : m_musicPlayerB;
}

MusicPlayer& EngineAudio::GetStandbyPlayer()
{
	return m_bSideA ? m_musicPlayerB : m_musicPlayerA;
}

const MusicPlayer& EngineAudio::GetActivePlayer() const
{
	return m_bSideA ? m_musicPlayerA : m_musicPlayerB;
}

const MusicPlayer& EngineAudio::GetStandbyPlayer() const
{
	return m_bSideA ? m_musicPlayerB : m_musicPlayerA;
}

void EngineAudio::PauseAll()
{
	GetActivePlayer().Pause();
	for (auto& sfxPlayer : m_sfxPlayers)
	{
		sfxPlayer->Pause();
	}
}

void EngineAudio::ResumeAll()
{
	GetActivePlayer().Resume();
	for (auto& sfxPlayer : m_sfxPlayers)
	{
		sfxPlayer->Resume();
	}
}

} // namespace LittleEngine
