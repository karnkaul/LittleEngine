#pragma once
#include "Core/CoreTypes.h"
#include "Core/Logger.h" // Logger::Severity
#include "Core/SimpleTime.h"

namespace Core
{
class GData;
}

namespace LittleEngine
{
using LogSeverity = Core::LogSeverity;

// \brief Wrapper to maintain properties saved to / loaded from a file
class GameConfig
{
private:
	// Set this to determine whether the engine will pause ticking if the window loses focus
	static const bool s_bPauseOnFocusLoss;

public:
	bool m_bRenderThread = true;

private:
	UPtr<Core::GData> m_uData;
	bool m_bDirty = false;
	
public:
	GameConfig();
	~GameConfig();

	void Init();

	// Load config file from path and replace cache values if valid
	bool Load(String path);
	// Save cache to config file at path; loadFirst will call Load() before saving
	bool Save(String path);

	bool ShouldCreateRenderThread() const;
	bool ShouldPauseOnFocusLoss() const;
	u32 JobWorkerCount() const;
	Time TickRate() const;
	Time RenderThreadStartDelay() const;
	Time MaxFrameTime() const;
	String TitleBarText() const;
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
