#pragma once
#include "Core/CoreTypes.h"
#include "Core/Game/GData.h"

namespace Core
{
class GData;
}

namespace LittleEngine
{
// \brief Wrapper to maintain properties saved to / loaded from a file
class GameConfig
{
private:
	// Set this to determine whether the engine will pause ticking if the window loses focus
	static const bool s_bPauseOnFocusLoss;

public:
	bool m_bRenderThread = true;

private:
	Core::GData m_data;
	bool m_bDirty = false;
	bool m_bInit = false;

public:
	static GameConfig& Instance();

public:
	void Init();

	// Load config file from path and replace cache values if valid
	bool Load(std::string path);
	// Save cache to config file at path; loadFirst will call Load() before saving
	bool Save(std::string path);

	bool ShouldCreateRenderThread() const;
	u32 JobWorkerCount() const;
	Time StepRate() const;
	Time TickRate() const;
	Time RenderThreadStartDelay() const;
	Time MaxFrameTime() const;
	std::string TitleBarText() const;
	u16 ColliderBorderWidth() const;
	Vector2 UISpace() const;
	Fixed WorldHeight() const;
	u8 BackupLogFileCount() const;
	Vector2 EntityOrientationSize() const;
	Vector2 ControllerOrientationSize() const;
	Fixed ControllerOrientationEpsilon() const;
	f64 ForceViewportAR() const;

private:
	void Verify();
};
} // namespace LittleEngine
