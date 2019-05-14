#include "stdafx.h"
#include "Core/GData.h"
#include "Core/Utils.h"
#include "EngineConfig.h"
#include "OS.h"

using GData = Core::GData;
using Version = Core::Version;

namespace LittleEngine
{
namespace
{
const char* RENDER_THREAD_KEY = "renderThread";
const char* PAUSE_ON_FOCUS_LOSS_KEY = "bPauseOnFocusLoss";
const char* NUM_GAME_THREADS_KEY = "numGameThreads";
const char* TICKS_PER_SECOND_KEY = "ticksPerSecond";
const char* MAX_TICK_TIME_MS = "maxTickTimeMS";
const char* WINDOW_TITLE_KEY = "windowTitle";
const char* LOG_LEVEL_KEY = "logLevel";
const char* VIEW_SIZE_KEY = "viewSize";
const char* COLLIDER_SHAPE_WIDTH_KEY = "colliderShapeBorderWidth";
const char* BACKUP_LOG_FILE_COUNT_KEY = "backupLogFileCount";

Version engineVersion = "0.1.6";

UMap<Core::LogSeverity, String> severityMap = {{Core::LogSeverity::Error, "Error"},
											   {Core::LogSeverity::Warning, "Warning"},
											   {Core::LogSeverity::Info, "Info"},
											   {Core::LogSeverity::Debug, "Debug"},
											   {Core::LogSeverity::HOT, "HOT"}};

Core::LogSeverity ParseLogLevel(String str)
{
	for (const auto& severity : severityMap)
	{
		if (severity.second == str)
		{
			return severity.first;
		}
	}
	return Core::LogSeverity::Info;
}

void SetStringIfEmpty(GData& data, String key, String value)
{
	if (data.GetString(key, "NULL") == "NULL")
	{
		data.SetString(key, std::move(value));
	}
}
} // namespace

const bool EngineConfig::s_bPauseOnFocusLoss = false;

EngineConfig::EngineConfig()
{
	m_uData = MakeUnique<GData>();
	Verify();
}

EngineConfig::~EngineConfig() = default;

bool EngineConfig::Load(String path)
{
	m_bDirty = true;
	FileRW file(std::move(path));
	m_uData->Marshall(file.ReadAll(true));
	bool success = m_uData->NumFields() > 0;
	Verify();
	return success;
}

bool EngineConfig::Save(String path)
{
	FileRW file(std::move(path));
	if (m_bDirty || !file.Exists())
	{
		return file.Write(m_uData->Unmarshall());
	}
	return true;
}

bool EngineConfig::ShouldCreateRenderThread() const
{
	return m_uData->GetBool(RENDER_THREAD_KEY);
}

bool EngineConfig::ShouldPauseOnFocusLoss() const
{
	return m_uData->GetBool(PAUSE_ON_FOCUS_LOSS_KEY);
}

u32 EngineConfig::GetNumGameThreads() const
{
	return static_cast<u32>(m_uData->GetS32(NUM_GAME_THREADS_KEY));
}

u32 EngineConfig::GetTicksPerSecond() const
{
	return static_cast<u32>(m_uData->GetS32(TICKS_PER_SECOND_KEY));
}

u32 EngineConfig::GetMaxTickTimeMS() const
{
	return static_cast<u32>(m_uData->GetS32(MAX_TICK_TIME_MS));
}

String EngineConfig::GetWindowTitle() const
{
	return m_uData->GetString(WINDOW_TITLE_KEY, "(Value unset in Engine Config GData file)");
}

u32 EngineConfig::GetColliderBorderWidth() const
{
	return static_cast<u32>(m_uData->GetS32(COLLIDER_SHAPE_WIDTH_KEY));
}

Core::LogSeverity EngineConfig::GetLogLevel() const
{
	return ParseLogLevel(m_uData->GetString(LOG_LEVEL_KEY));
}

Vector2 EngineConfig::GetViewSize() const
{
	GData vec2 = m_uData->GetGData(VIEW_SIZE_KEY);
	return Vector2(Fixed(vec2.GetS32("x")), Fixed(vec2.GetS32("y")));
}


u8 EngineConfig::GetBackupLogFileCount() const
{
	return static_cast<u8>(m_uData->GetS32(BACKUP_LOG_FILE_COUNT_KEY));
}

const Version& EngineConfig::GetEngineVersion()
{
	Version fileEngineVersion = OS::Env()->GetFileEngineVersion();
	if (fileEngineVersion != Version())
	{
		engineVersion = fileEngineVersion;
	}
	return engineVersion;
}

bool EngineConfig::SetCreateRenderThread(bool bCreate)
{
	return m_bDirty = m_uData->SetString(RENDER_THREAD_KEY, Strings::ToString(bCreate));
}

bool EngineConfig::SetPauseOnFocusLoss(bool bPause)
{
	return m_bDirty = m_uData->SetString(PAUSE_ON_FOCUS_LOSS_KEY, Strings::ToString(bPause));
}

bool EngineConfig::SetNumGameThreads(u32 numThreads)
{
	return m_bDirty = m_uData->SetString(NUM_GAME_THREADS_KEY, Strings::ToString(numThreads));
	;
}

bool EngineConfig::SetTicksPerSecond(u32 ticksPerSecond)
{
	return m_bDirty = m_uData->SetString(TICKS_PER_SECOND_KEY, Strings::ToString(ticksPerSecond));
}

bool EngineConfig::SetMaxTimeMS(u32 maxTickTimeMS)
{
	return m_bDirty = m_uData->SetString(MAX_TICK_TIME_MS, Strings::ToString(maxTickTimeMS));
}

bool EngineConfig::SetWindowTitle(String windowTitle)
{
	return m_bDirty = m_uData->SetString(WINDOW_TITLE_KEY, std::move(windowTitle));
}

bool EngineConfig::SetLogLevel(LogSeverity level)
{
	return m_bDirty = m_uData->SetString(LOG_LEVEL_KEY, severityMap[level]);
}

bool EngineConfig::SetColliderBorderWidth(u32 shapeWidth)
{
	return m_bDirty = m_uData->SetString(COLLIDER_SHAPE_WIDTH_KEY, Strings::ToString(shapeWidth));
}

bool EngineConfig::SetViewSize(u32 width, u32 height)
{
	GData gData;
	gData.SetString("x", Strings::ToString(width));
	gData.SetString("y", Strings::ToString(height));
	return m_bDirty = m_uData->AddField(VIEW_SIZE_KEY, gData);
}


bool EngineConfig::SetBackupLogFileCount(u8 count)
{
	return m_bDirty = m_uData->SetString(BACKUP_LOG_FILE_COUNT_KEY, Strings::ToString(count));
}

void EngineConfig::Verify()
{
	u32 backupLogFileCount = 5;
#if DEBUGGING
	backupLogFileCount = 3;
#endif
	SetStringIfEmpty(*m_uData, WINDOW_TITLE_KEY, "Async Little Engine");
	SetStringIfEmpty(*m_uData, LOG_LEVEL_KEY, "Info");
	SetStringIfEmpty(*m_uData, COLLIDER_SHAPE_WIDTH_KEY, "2");
	SetStringIfEmpty(*m_uData, TICKS_PER_SECOND_KEY, Strings::ToString(60));
	SetStringIfEmpty(*m_uData, MAX_TICK_TIME_MS, Strings::ToString(25));
	SetStringIfEmpty(*m_uData, RENDER_THREAD_KEY, Strings::ToString(true));
	SetStringIfEmpty(*m_uData, NUM_GAME_THREADS_KEY, Strings::ToString(6));
	SetStringIfEmpty(*m_uData, PAUSE_ON_FOCUS_LOSS_KEY, Strings::ToString(s_bPauseOnFocusLoss));
	SetStringIfEmpty(*m_uData, BACKUP_LOG_FILE_COUNT_KEY, Strings::ToString(backupLogFileCount));
	if (m_uData->GetString(VIEW_SIZE_KEY).empty())
	{
		SetViewSize(1920, 1080);
	}
	m_bDirty = false;
}
} // namespace LittleEngine
