#include "Core/LECoreGame/ArchiveReader.h"
#include "Core/LECoreGame/OS.h"
#include "Core/LECoreGame/LECoreUtils/Utils.h"
#include "SFMLAPI/System/Assets.h"
#include "LEAudio.h"
#include "Engine/Repository/LERepository.h"
#include "Engine/Debug/Tweakable.h"
#if defined(DEBUGGING)
#include "SFMLAPI/Rendering/Primitives/Quad.h"
#include "SFMLAPI/Rendering/Primitives/SFText.h"
#include "Engine/GFX.h"
#include "Engine/Rendering/LERenderer.h"
#endif

namespace LittleEngine
{
namespace
{
#if defined(DEBUGGING)
std::string GetStatus(AudioPlayer::Status status)
{
	switch (status)
	{
	case AudioPlayer::Status::NoMedia:
		return "x";
	case AudioPlayer::Status::Paused:
		return "|";
	case AudioPlayer::Status::Playing:
		return ">";
	case AudioPlayer::Status::Stopped:
		return ".";
	}
	return "?";
}
#endif
} // namespace

#if defined(DEBUGGING)
struct LEAudio::DbgImpl final
{
	static constexpr u32 ID_SIZE = 17;
	static constexpr Fixed VOL_H = Fixed(3, 1);
	static constexpr Fixed PADL = Fixed(10, 1);
	static constexpr Fixed VOL_PADR = Fixed(50, 1);
	static constexpr Fixed TEXT_Y = Fixed(15, 1);
	const Vector2 size = Vector2(350, 50);
	std::string idA;
	std::string statusA;
	std::string idB;
	std::string statusB;
	Fixed volA;
	Fixed volB;
	Colour colA;
	Colour colB;
	Quad* pBGA = nullptr;
	Quad* pVolA = nullptr;
	SFText* pIDA = nullptr;
	SFText* pStatusA = nullptr;
	Quad* pBGB = nullptr;
	Quad* pVolB = nullptr;
	SFText* pIDB = nullptr;
	SFText* pStatusB = nullptr;

	DbgImpl(LERenderer& r);
	~DbgImpl();

	void Tick(Time dt);
};

LEAudio::DbgImpl::DbgImpl(LERenderer& r)
{
	const Vector2 overlay = g_pGFX->OverlaySpace();
	const Vector2 posA = Fixed::OneHalf * (overlay - size) - Vector2(30, 50);
	pBGA = r.New<Quad>(LayerID::TopOverlay);
	pBGA->SetModel(Rect2::SizeCentre(size), true)->SetColour(Colour(50, 50, 50, 50), true)->SetPosition(posA, true);
	pIDA = r.New<SFText>(LayerID::TopOverlay);
	pIDA->SetFont(*g_pDefaultFont)
		->SetSize(ID_SIZE)
		->SetPivot({-1, 0})
		->SetColour(Colour::White)
		->SetPosition(posA + Vector2(-size.x / 2 + PADL, size.y / 2 - TEXT_Y));
	pStatusA = r.New<SFText>(LayerID::TopOverlay);
	pStatusA->SetFont(*g_pDefaultFont)->SetPivot({-1, 0})->SetColour(Colour::White)->SetPosition(posA + Vector2(size.x / 2 - 30, 0));
	const Vector2 tl(PADL, VOL_H);
	const Vector2 br(size.x - VOL_PADR + PADL, -VOL_H);
	pVolA = r.New<Quad>(LayerID::TopOverlay);
	pVolA->SetModel({tl, br}, true)->SetColour(Colour::White)->SetPosition(posA - Vector2(size.x / 2, size.y / 2 - TEXT_Y));

	const Vector2 posB(posA.x, posA.y - size.y);
	pBGB = r.New<Quad>(LayerID::TopOverlay);
	pBGB->SetModel(Rect2::SizeCentre(size), true)->SetColour(Colour(50, 50, 50, 50), true)->SetPosition(posB, true);
	pIDB = r.New<SFText>(LayerID::TopOverlay);
	pIDB->SetFont(*g_pDefaultFont)
		->SetSize(ID_SIZE)
		->SetPivot({-1, 0})
		->SetColour(Colour::White)
		->SetPosition(posB + Vector2(-size.x / 2 + PADL, size.y / 2 - TEXT_Y));
	pStatusB = r.New<SFText>(LayerID::TopOverlay);
	pStatusB->SetFont(*g_pDefaultFont)->SetPivot({-1, 0})->SetColour(Colour::White)->SetPosition(posB + Vector2(size.x / 2 - 30, 0));
	pVolB = r.New<Quad>(LayerID::TopOverlay);
	pVolB->SetModel({tl, br}, true)->SetColour(Colour::White)->SetPosition(posB - Vector2(size.x / 2, size.y / 2 - TEXT_Y));
}

LEAudio::DbgImpl::~DbgImpl()
{
	std::vector<APrimitive*> prims = {pVolA, pVolB, pIDA, pIDB, pStatusA, pStatusB, pBGA, pBGB};
	for (auto pPrim : prims)
	{
		if (pPrim)
		{
			pPrim->Destroy();
		}
	}
}

void LEAudio::DbgImpl::Tick(Time /*dt*/)
{
	std::vector<APrimitive*> prims = {pVolA, pVolB, pIDA, pIDB, pStatusA, pStatusB, pBGA, pBGB};
	for (auto pPrim : prims)
	{
		if (pPrim)
		{
			pPrim->SetEnabled(LEAudio::s_bDebugHUD);
		}
	}
	pIDA->SetText(idA)->SetColour(colA, false);
	pIDB->SetText(idB)->SetColour(colB, false);
	pStatusA->SetText(statusA)->SetColour(colA, false);
	pStatusB->SetText(statusB)->SetColour(colB, false);
	const Vector2 tl(PADL, VOL_H);
	Vector2 br(volA * (size.x - VOL_PADR) + PADL, -VOL_H);
	pVolA->SetModel({tl, br}, false)->SetColour(colA, false);
	br.x = volB * (size.x - VOL_PADR) + PADL;
	pVolB->SetModel({tl, br}, false)->SetColour(colB, false);
}
#endif

LEAudio::SwitchTrackRequest::SwitchTrackRequest(std::string newTrackPath, Time fadeTime, Fixed targetVolume, bool bLoop)
	: newTrackPath(std::move(newTrackPath)), fadeTime(fadeTime), targetVolume(targetVolume), bLoop(bLoop)
{
}

LEAudio* g_pAudio = nullptr;

#if ENABLED(TWEAKABLES)
TweakF32(masterVol, nullptr);
TweakF32(musicVol, nullptr);
TweakF32(sfxVol, nullptr);
#endif

#if defined(DEBUGGING)
bool LEAudio::s_bDebugHUD = false;
TweakBool(audioDebug, &LEAudio::s_bDebugHUD);
#endif

LEAudio::LEAudio()
{
	LOG_D("[Audio] constructed");
	g_pAudio = this;
#if ENABLED(TWEAKABLES)
	masterVol.BindCallback(
		[](std::string_view val) { AudioPlayer::s_mixVols[ToIdx(AudioPlayer::Mix::Master)] = Fixed(Strings::ToF32(std::string(val))); });
	musicVol.BindCallback([](std::string_view val) { AudioPlayer::s_mixVols[ToIdx(AudioPlayer::Mix::Music)] = Fixed(Strings::ToF32(std::string(val))); });
	sfxVol.BindCallback([](std::string_view val) { AudioPlayer::s_mixVols[ToIdx(AudioPlayer::Mix::SFX)] = Fixed(Strings::ToF32(std::string(val))); });
#endif
}

LEAudio::~LEAudio()
{
	g_pAudio = nullptr;
	Clear();
	LOG_D("[Audio] destroyed");
}

void LEAudio::Tick(Time dt)
{
	MusicPlayer* pActive = &ActivePlayer();
	MusicPlayer* pStandby = &StandbyPlayer();
	if (m_oSwitchTrackRequest)
	{
		if (pActive->IsPlaying())
		{
			pStandby->Stop();
			pActive->FadeOut(m_oSwitchTrackRequest->fadeTime);
			SwitchSide(pActive, pStandby);
		}
		pActive->m_bLooping = m_oSwitchTrackRequest->bLoop;
		pActive->SetTrack(std::move(m_oSwitchTrackRequest->newTrackPath));
		pActive->FadeIn(m_oSwitchTrackRequest->fadeTime, m_oSwitchTrackRequest->targetVolume);
		pActive->Play();
		m_oSwitchTrackRequest.reset();
	}
	pActive->Tick(dt);
	pStandby->Tick(dt);
	for (auto& sfxPlayer : m_sfxPlayers)
	{
		sfxPlayer->Tick(dt);
	}
#if defined(DEBUGGING)
	if (m_uDbg)
	{
		m_uDbg->colA = m_uDbg->colB = Colour::White;
		if (IsMusicPlaying())
		{
			m_bSideA ? m_uDbg->colA = Colour::Green : m_uDbg->colB = Colour::Green;
		}
		m_uDbg->statusA = GetStatus(m_musicPlayerA.m_status) + (m_musicPlayerA.m_bLooping ? "o" : "");
		m_uDbg->statusB = GetStatus(m_musicPlayerB.m_status) + (m_musicPlayerB.m_bLooping ? "o" : "");
		m_uDbg->volA = m_musicPlayerA.m_volume;
		m_uDbg->volB = m_musicPlayerB.m_volume;
		m_uDbg->Tick(dt);
	}
#endif
}

void LEAudio::SetMixVolume(AudioPlayer::Mix mix, Fixed nVol)
{
	auto idx = ToIdx(mix);
	Assert(idx < AudioPlayer::s_mixVols.size(), "Invalid mix!");
	if (idx < AudioPlayer::s_mixVols.size())
	{
		AudioPlayer::s_mixVols[idx] = Maths::Clamp01(nVol);
	}
}

SoundPlayer* LEAudio::PlaySFX(std::string id, Fixed nVol, Fixed direction, bool bLoop)
{
	Assert(g_pRepository, "Repository is null!");
	SoundPlayer& sfxPlayer = GetOrCreateSFXPlayer();
	auto pAsset = g_pRepository->Load<SoundAsset>(std::move(id));
	if (pAsset)
	{
		sfxPlayer.SetSoundAsset(*pAsset);
		sfxPlayer.m_volume = nVol;
		sfxPlayer.m_bLooping = bLoop;
		sfxPlayer.SetDirection(direction);
		sfxPlayer.Play();
		return &sfxPlayer;
	}
	return nullptr;
}

SoundPlayer* LEAudio::PlaySFX(SoundAsset& sound, Fixed nVol, Fixed direction, bool bLoop)
{
	SoundPlayer& sfxPlayer = GetOrCreateSFXPlayer();
	sfxPlayer.SetSoundAsset(sound);
	sfxPlayer.m_volume = nVol;
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

bool LEAudio::PlayMusic(std::string id, Fixed nVol, Time fadeTime, bool bLoop)
{
	m_oSwitchTrackRequest.reset();
	MusicPlayer& active = ActivePlayer();
	active.m_bLooping = bLoop;
	if (active.IsPlaying())
	{
		active.Stop();
	}
#if defined(DEBUGGING)
	(m_uDbg && m_bSideA) ? m_uDbg->idA = id : m_uDbg->idB = id;
#endif
	bool bSuccess = active.SetTrack(GetPath(std::move(id)));
	if (bSuccess)
	{
		if (fadeTime.AsSeconds() > 0)
		{
			active.FadeIn(fadeTime, nVol);
		}
		else
		{
			active.m_volume = nVol;
			active.Play();
		}
	}
	return bSuccess;
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

void LEAudio::PauseMusic(Time fadeTime)
{
	MusicPlayer& player = ActivePlayer();
	if (fadeTime > Time::Zero)
	{
		player.m_bPauseOnFadeOut = true;
		player.FadeOut(fadeTime);
	}
	else
	{
		player.Pause();
	}
}

bool LEAudio::IsMusicPaused() const
{
	return ActivePlayer().m_status == AudioPlayer::Status::Paused;
}

bool LEAudio::ResumeMusic(Time fadeTime, Fixed nVol)
{
	MusicPlayer& player = ActivePlayer();
	if (player.IsPaused())
	{
		if (fadeTime > Time::Zero)
		{
			player.FadeIn(fadeTime, nVol);
		}
		else
		{
			player.m_volume = nVol;
			player.Play();
		}
		return true;
	}
	return false;
}

void LEAudio::SwitchTrack(std::string id, Fixed nVol, Time fadeTime, bool bLoop)
{
#if defined(DEBUGGING)
	(m_uDbg && !m_bSideA) ? m_uDbg->idA = id : m_uDbg->idB = id;
#endif
	m_oSwitchTrackRequest.emplace(GetPath(std::move(id)), fadeTime, nVol, bLoop);
}

void LEAudio::SetMusicVolume(Fixed nVol)
{
	if (m_oSwitchTrackRequest)
	{
		m_oSwitchTrackRequest->targetVolume = nVol;
	}
	else
	{
		MusicPlayer& active = ActivePlayer();
		if (active.IsFading())
		{
			active.m_targetVolume = nVol;
		}
		else
		{
			active.m_volume = nVol;
		}
	}
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
	m_oSwitchTrackRequest.reset();
	LOG_D("[Audio] Cleared [%d] SFXPlayers, 1 MusicPlayer]", sfxCount);
}

#if defined(DEBUGGING)
void LEAudio::InitDebug(LERenderer& renderer)
{
	m_uDbg = std::make_unique<DbgImpl>(renderer);
}

void LEAudio::DestroyDebug()
{
	m_uDbg = nullptr;
}
#endif

SoundPlayer& LEAudio::GetOrCreateSFXPlayer()
{
	for (auto& sfxPlayer : m_sfxPlayers)
	{
		if (!sfxPlayer->IsPlaying())
		{
			return *sfxPlayer;
		}
	}
	m_sfxPlayers.push_back(std::make_unique<SoundPlayer>(nullptr));
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

void LEAudio::SwitchSide(MusicPlayer*& out_pActive, MusicPlayer*& out_pStandby)
{
	m_bSideA = !m_bSideA;
	out_pActive = &ActivePlayer();
	out_pStandby = &StandbyPlayer();
}

std::string LEAudio::GetPath(std::string id) const
{
	std::string ret;
	ret.reserve(256);
	ret += OS::Env()->RuntimePath();
	ret += "/";
	ret += m_rootMusicDir;
	ret += "/";
	ret += id;
	return ret;
}
} // namespace LittleEngine
