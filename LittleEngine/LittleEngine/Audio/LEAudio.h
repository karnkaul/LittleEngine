#pragma once
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
		Time fadeTime;
		Fixed targetVolume;
		String newTrackPath;
		bool bFadingOldTrack = false;
		bool bFadingNewTrack = false;

		SwitchTrackRequest(String  newTrackPath, Time fadeTime, Fixed  targetVolume)
			: fadeTime(fadeTime), targetVolume(std::move(targetVolume)), newTrackPath(std::move(newTrackPath))
		{
		}
	};

private:
	MusicPlayer m_musicPlayerA;
	MusicPlayer m_musicPlayerB;
	Vec<UPtr<SoundPlayer>> m_sfxPlayers;
	UPtr<SwitchTrackRequest> m_uSwitchTrackRequest;
	bool m_bSideA = true;
	const String m_rootMusicDir = "GameMusic";

public:
	LEAudio();
	~LEAudio();

	void Tick(Time dt);
	
	// Returns nullptr if asset could not be loaded
	SoundPlayer* PlaySFX(String id,
						 Fixed volume = Fixed::One,
						 Fixed direction = Fixed::Zero,
						 bool bLoop = false);
	SoundPlayer* PlaySFX(class SoundAsset& sound,
						 Fixed volume = Fixed::One,
						 Fixed direction = Fixed::Zero,
						 bool bLoop = false);
	bool IsSFXPlaying() const;

	// Returns true if asset is loaded successfully
	bool PlayMusic(String id, Fixed volume = Fixed::One, Time fadeTime = Time::Seconds(1), bool bLoop = true);
	bool IsMusicPlaying() const;
	void StopMusic(Time fadeTime = Time::Zero);
	bool ResumeMusic(Time fadeTime = Time::Zero, Fixed volume = Fixed::One);
	void SwitchTrack(String id, Fixed volume = Fixed::One, Time fadeTime = Time::Seconds(1));
	void SetMusicVolume(Fixed volume);

	void PauseAll();
	void ResumeAll();

	// Stop all audio and flush all SFX players
	void Clear(bool bImmediate = true);

private:
	SoundPlayer& GetOrCreateSFXPlayer();
	MusicPlayer& GetActivePlayer();
	MusicPlayer& GetStandbyPlayer();
	const MusicPlayer& GetActivePlayer() const;
	const MusicPlayer& GetStandbyPlayer() const;

	String GetPath(String id) const;

	friend class LEContext;
};
} // namespace LittleEngine
