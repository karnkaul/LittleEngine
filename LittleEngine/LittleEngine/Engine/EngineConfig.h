#pragma once
#include "Core/CoreTypes.h"
#include "Core/Logger.h" // Logger::Severity
#include "Core/Version.h"

namespace Core
{
class GData;
}

namespace LittleEngine
{
using LogSeverity = Core::LogSeverity;
using Version = Core::Version;

// \brief Wrapper to maintain properties saved to / loaded from config.ini
class EngineConfig
{
private:
	// Set this to determine whether the engine will pause ticking if the window loses focus
	static const bool s_bPauseOnFocusLoss;
	
	UPtr<Core::GData> m_uData;
	bool m_bDirty = false;

public:
	static const Version& GetEngineVersion();

public:
	EngineConfig();
	~EngineConfig();

	// Load config file from path and replace cache values if valid
	bool Load(String path);
	// Save cache to config file at path; loadFirst will call Load() before saving
	bool Save(String path);

	bool ShouldCreateRenderThread() const;
	bool ShouldPauseOnFocusLoss() const;
	u32 GetNumGameThreads() const;
	u32 GetTicksPerSecond() const;
	u32 GetMaxTickTimeMS() const;
	String GetWindowTitle() const;
	u32 GetColliderBorderWidth() const;
	LogSeverity GetLogLevel() const;
	Vector2 GetViewSize() const;
	u8 GetBackupLogFileCount() const;

	bool SetCreateRenderThread(bool bCreate);
	bool SetPauseOnFocusLoss(bool bPause);
	bool SetNumGameThreads(u32 numThreads);
	bool SetTicksPerSecond(u32 ticksPerSecond);
	bool SetMaxTimeMS(u32 maxTickTimeMS);
	bool SetWindowTitle(String windowTitle);
	bool SetLogLevel(LogSeverity level);
	bool SetColliderBorderWidth(u32 shapeWidth);
	bool SetViewSize(u32 width, u32 height);
	bool SetBackupLogFileCount(u8 count);

private:
	void Verify();
};
} // namespace LittleEngine
