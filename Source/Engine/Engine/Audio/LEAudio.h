#pragma once
#include <optional>
#include "Core/CoreTypes.h"
#include "SFMLAPI/Audio/AudioPlayer.h"

namespace LittleEngine
{
extern class LEAudio* g_pAudio;

// \brief Handler for all SFX and Music playback
class LEAudio final
{
private:
	// \brief Helper to store next track until current has faded out
	struct SwitchTrackRequest
	{
		std::string newTrackPath;
		Time fadeTime;
		Fixed targetVolume;
		bool bLoop = true;

		SwitchTrackRequest(std::string newTrackPath, Time fadeTime, Fixed targetVolume, bool bLoop);
	};

#if defined(DEBUGGING)
public:
	static bool s_bDebugHUD;
#endif

private:
	MusicPlayer m_musicPlayerA;
	MusicPlayer m_musicPlayerB;
	std::vector<std::unique_ptr<SoundPlayer>> m_sfxPlayers;
	std::optional<SwitchTrackRequest> m_oSwitchTrackRequest;
	bool m_bSideA = true;
	const std::string m_rootMusicDir = "GameMusic";

public:
	LEAudio();
	~LEAudio();

	void Tick(Time dt);
	void SetMixVolume(AudioPlayer::Mix mix, Fixed nVol);

	// Returns nullptr if asset could not be loaded
	SoundPlayer* PlaySFX(std::string id, Fixed nVol = Fixed::One, Fixed direction = Fixed::Zero, bool bLoop = false);
	SoundPlayer* PlaySFX(class SoundAsset& sound, Fixed nVol = Fixed::One, Fixed direction = Fixed::Zero, bool bLoop = false);
	bool IsSFXPlaying() const;

	// Returns true if asset is loaded successfully
	bool PlayMusic(std::string id, Fixed nVol = Fixed::One, Time fadeTime = Time::Seconds(1), bool bLoop = true);
	bool IsMusicPlaying() const;
	void PauseMusic(Time fadeTime = Time::Seconds(0.5f));
	bool IsMusicPaused() const;
	bool ResumeMusic(Time fadeTime = Time::Seconds(0.5f), Fixed nVol = Fixed::One);
	void StopMusic(Time fadeTime = Time::Zero);
	void SwitchTrack(std::string id, Fixed nVol = Fixed::One, Time fadeTime = Time::Seconds(1), bool bLoop = true);
	void SetMusicVolume(Fixed nVol);

	void PauseAll();
	void ResumeAll();

	// Stop all audio and flush all SFX players
	void Clear(bool bImmediate = true);

#if defined(DEBUGGING)
	void InitDebug(class LERenderer& renderer);
	void DestroyDebug();
#endif

private:
	SoundPlayer& GetOrCreateSFXPlayer();
	MusicPlayer& ActivePlayer();
	MusicPlayer& StandbyPlayer();
	const MusicPlayer& ActivePlayer() const;
	const MusicPlayer& StandbyPlayer() const;
	void SwitchSide(MusicPlayer*& out_pActive, MusicPlayer*& out_pStandby);
	std::string GetPath(std::string id) const;

#if defined(DEBUGGING)
	struct DbgImpl;
	std::unique_ptr<DbgImpl> m_uDbg;
#endif
};
} // namespace LittleEngine
