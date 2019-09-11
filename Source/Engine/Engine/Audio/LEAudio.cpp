#include "Core/ArchiveReader.h"
#include "Core/Logger.h"
#include "Core/OS.h"
#include "Core/Utils.h"
#include "SFMLAPI/System/Assets.h"
#include "LEAudio.h"
#include "Engine/Repository/LERepository.h"

namespace LittleEngine
{
LEAudio* g_pAudio = nullptr;

LEAudio::LEAudio()
{
	LOG_D("[Audio] constructed");
	g_pAudio = this;
}

LEAudio::~LEAudio()
{
	g_pAudio = nullptr;
	Clear();
	LOG_D("[Audio] destroyed");
}

SoundPlayer* LEAudio::PlaySFX(String id, Fixed volume, Fixed direction, bool bLoop)
{
	Assert(g_pRepository, "Repository is null!");
	SoundPlayer& sfxPlayer = GetOrCreateSFXPlayer();
	auto pAsset = g_pRepository->Load<SoundAsset>(std::move(id));
	if (pAsset)
	{
		sfxPlayer.SetSoundAsset(*pAsset);
		sfxPlayer.m_volume = volume;
		sfxPlayer.m_bLooping = bLoop;
		sfxPlayer.SetDirection(direction);
		sfxPlayer.Play();
		return &sfxPlayer;
	}
	return nullptr;
}

SoundPlayer* LEAudio::PlaySFX(SoundAsset& sound, Fixed volume, Fixed direction, bool bLoop)
{
	SoundPlayer& sfxPlayer = GetOrCreateSFXPlayer();
	sfxPlayer.SetSoundAsset(sound);
	sfxPlayer.m_volume = volume;
	sfxPlayer.m_bLooping = bLoop;
	sfxPlayer.SetDirection(direction);
	sfxPlayer.Play();
	return &sfxPlayer;
}

bool LEAudio::IsSFXPlaying() const
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

bool LEAudio::PlayMusic(String id, Fixed volume, Time fadeTime, bool bLoop)
{
	m_uSwitchTrackRequest = nullptr;
	MusicPlayer& active = ActivePlayer();
	active.m_bLooping = bLoop;
	if (active.IsPlaying())
	{
		active.Stop();
	}
	active.SetTrack(GetPath(std::move(id)));
	if (fadeTime.AsSeconds() > 0)
	{
		active.FadeIn(fadeTime, volume);
	}
	else
	{
		active.m_volume = volume;
		active.Play();
	}
	return true;
}

bool LEAudio::IsMusicPlaying() const
{
	return ActivePlayer().IsPlaying();
}

void LEAudio::StopMusic(Time fadeTime)
{
	ActivePlayer().FadeOut(fadeTime, Fixed::Zero);
	StandbyPlayer().Stop();
}

bool LEAudio::ResumeMusic(Time fadeTime, Fixed volume)
{
	MusicPlayer& player = ActivePlayer();
	if (player.IsPaused())
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

void LEAudio::SwitchTrack(String id, Fixed volume, Time fadeTime)
{
	m_uSwitchTrackRequest = MakeUnique<SwitchTrackRequest>(GetPath(std::move(id)), fadeTime, volume);
	if (IsMusicPlaying())
	{
		ActivePlayer().FadeOut(fadeTime.Scale(Fixed::OneHalf));
		m_uSwitchTrackRequest->bFadingOldTrack = true;
		m_uSwitchTrackRequest->fadeTime.Scale(Fixed::OneHalf);
	}
}

void LEAudio::SetMusicVolume(Fixed volume)
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

void LEAudio::Clear(bool immediate)
{
#if ENABLED(DEBUG_LOGGING)
	u32 sfxCount = static_cast<u32>(m_sfxPlayers.size());
#endif
	for (auto& sfxPlayer : m_sfxPlayers)
	{
		sfxPlayer->Stop();
	}
	m_sfxPlayers.clear();
	MusicPlayer& active = ActivePlayer();
	if (immediate)
	{
		active.EndFade();
		active.Stop();
	}
	else if (active.IsPlaying())
	{
		active.FadeOut(Time::Seconds(0.5f));
	}
	StandbyPlayer().Stop();
	m_uSwitchTrackRequest = nullptr;
	LOG_D("[Audio] Cleared [%d] SFXPlayers, 1 MusicPlayer]", sfxCount);
}

void LEAudio::Tick(Time dt)
{
	MusicPlayer& active = ActivePlayer();
	if (m_uSwitchTrackRequest != nullptr && !active.IsFading())
	{
		if (active.IsPlaying())
		{
			active.Stop();
		}
		active.SetTrack(std::move(m_uSwitchTrackRequest->newTrackPath));
		active.FadeIn(m_uSwitchTrackRequest->fadeTime, m_uSwitchTrackRequest->targetVolume);
		m_uSwitchTrackRequest = nullptr;
	}
	active.Tick(dt);
	StandbyPlayer().Tick(dt);
	for (auto& sfxPlayer : m_sfxPlayers)
	{
		sfxPlayer->Tick(dt);
	}
}

String LEAudio::GetPath(String id) const
{
	static Array<char, 256> buf;
	SPRINTF(buf.data(), buf.size(), "%s/%s/%s", OS::Env()->RuntimePath().c_str(), m_rootMusicDir.c_str(), id.c_str());
	return String(buf.data());
}

SoundPlayer& LEAudio::GetOrCreateSFXPlayer()
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

MusicPlayer& LEAudio::ActivePlayer()
{
	return m_bSideA ? m_musicPlayerA : m_musicPlayerB;
}

MusicPlayer& LEAudio::StandbyPlayer()
{
	return m_bSideA ? m_musicPlayerB : m_musicPlayerA;
}

const MusicPlayer& LEAudio::ActivePlayer() const
{
	return m_bSideA ? m_musicPlayerA : m_musicPlayerB;
}

const MusicPlayer& LEAudio::StandbyPlayer() const
{
	return m_bSideA ? m_musicPlayerB : m_musicPlayerA;
}

void LEAudio::PauseAll()
{
	ActivePlayer().Pause();
	for (auto& sfxPlayer : m_sfxPlayers)
	{
		sfxPlayer->Pause();
	}
}

void LEAudio::ResumeAll()
{
	ActivePlayer().Resume();
	for (auto& sfxPlayer : m_sfxPlayers)
	{
		sfxPlayer->Resume();
	}
}

} // namespace LittleEngine
