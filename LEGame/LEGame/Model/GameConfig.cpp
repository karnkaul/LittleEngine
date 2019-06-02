#include "stdafx.h"
#include "Core/GData.h"
#include "Core/Utils.h"
#include "LittleEngine/OS.h"
#include "GameConfig.h"

using GData = Core::GData;
using Version = Core::Version;

namespace LittleEngine
{
namespace
{
const char* RENDER_THREAD_KEY = "bRenderThread";
const char* PAUSE_ON_FOCUS_LOSS_KEY = "bPauseOnFocusLoss";
const char* JOB_WORKER_COUNT_KEY = "jobWorkerCount";
const char* TICKS_PER_SECOND_KEY = "ticksPerSecond";
const char* RENDER_THREAD_START_DELAY_MS_KEY = "renderThreadStartDelayMS";
const char* MAX_TICK_TIME_MS = "maxTickTimeMS";
const char* TITLEBAR_TEXT_KEY = "titleBarText";
const char* LOG_LEVEL_KEY = "logLevel";
const char* VIEW_SIZE_KEY = "viewSize";
const char* COLLIDER_SHAPE_WIDTH_KEY = "colliderShapeBorderWidth";
const char* BACKUP_LOG_FILE_COUNT_KEY = "backupLogFileCount";

Version engineVersion = "0.1.6";
Version gameVersion = "0.0.1";

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

bool SetStringIfEmpty(GData& data, String key, String value)
{
	if (data.GetString(key, "NULL") == "NULL")
	{
		data.SetString(key, std::move(value));
		return true;
	}
	return false;
}
} // namespace

const bool GameConfig::s_bPauseOnFocusLoss = false;

GameConfig::GameConfig() = default;
GameConfig::~GameConfig() = default;

void GameConfig::Init()
{
	m_uData = MakeUnique<GData>();
	Verify();
}

bool GameConfig::Load(String path)
{
	m_bDirty = true;
	FileRW file(std::move(path));
	m_uData->Marshall(file.ReadAll(true));
	bool success = m_uData->NumFields() > 0;
	Verify();
	return success;
}

bool GameConfig::Save(String path)
{
	FileRW file(std::move(path));
	if (m_bDirty || !file.Exists())
	{
		return file.Write(m_uData->Unmarshall());
	}
	return true;
}

bool GameConfig::ShouldCreateRenderThread() const
{
	return m_uData->GetBool(RENDER_THREAD_KEY);
}

bool GameConfig::ShouldPauseOnFocusLoss() const
{
	return m_uData->GetBool(PAUSE_ON_FOCUS_LOSS_KEY);
}

u32 GameConfig::GetJobWorkerCount() const
{
	return static_cast<u32>(m_uData->GetS32(JOB_WORKER_COUNT_KEY));
}

Time GameConfig::GetTickRate() const
{
	u32 ticks = static_cast<u32>(m_uData->GetS32(TICKS_PER_SECOND_KEY));
	return Time::Seconds(1.0f / ticks);
}

Time GameConfig::GetRenderThreadStartDelay() const
{
	return Time::Milliseconds(static_cast<u32>(m_uData->GetS32(RENDER_THREAD_START_DELAY_MS_KEY)));
}

Time GameConfig::GetMaxTickTime() const
{
	return Time::Milliseconds(static_cast<u32>(m_uData->GetS32(MAX_TICK_TIME_MS)));
}

String GameConfig::GetTitleBarText() const
{
	return m_uData->GetString(TITLEBAR_TEXT_KEY, "(Value unset in Engine Config GData file)");
}

u32 GameConfig::GetColliderBorderWidth() const
{
	return static_cast<u32>(m_uData->GetS32(COLLIDER_SHAPE_WIDTH_KEY));
}

Core::LogSeverity GameConfig::GetLogLevel() const
{
	return ParseLogLevel(m_uData->GetString(LOG_LEVEL_KEY));
}

Vector2 GameConfig::GetViewSize() const
{
	return m_uData->GetVector2(VIEW_SIZE_KEY);
}

u8 GameConfig::GetBackupLogFileCount() const
{
	return static_cast<u8>(m_uData->GetS32(BACKUP_LOG_FILE_COUNT_KEY));
}

const Version& GameConfig::GetEngineVersion()
{
	Version fileEngineVersion = OS::Env()->GetFileEngineVersion();
	if (fileEngineVersion != Version())
	{
		engineVersion = fileEngineVersion;
	}
	return engineVersion;
}

const Version& GameConfig::GetGameVersion()
{
	Version fileGameVersion = OS::Env()->GetFileGameVersion();
	if (fileGameVersion != Version())
	{
		gameVersion = fileGameVersion;
	}
	return gameVersion;
}

bool GameConfig::SetCreateRenderThread(bool bCreate)
{
	return m_bDirty = m_uData->SetString(RENDER_THREAD_KEY, Strings::ToString(bCreate));
}

bool GameConfig::SetPauseOnFocusLoss(bool bPause)
{
	return m_bDirty = m_uData->SetString(PAUSE_ON_FOCUS_LOSS_KEY, Strings::ToString(bPause));
}

bool GameConfig::SetJobWorkerCount(u32 numThreads)
{
	return m_bDirty = m_uData->SetString(JOB_WORKER_COUNT_KEY, Strings::ToString(numThreads));
}

bool GameConfig::SetTicksPerSecond(u32 ticksPerSecond)
{
	return m_bDirty = m_uData->SetString(TICKS_PER_SECOND_KEY, Strings::ToString(ticksPerSecond));
}


bool GameConfig::SetRenderThreadStartDelay(u32 delayMS)
{
	return m_bDirty = m_uData->SetString(RENDER_THREAD_START_DELAY_MS_KEY, Strings::ToString(delayMS));
}

bool GameConfig::SetMaxTimeMS(u32 maxTickTimeMS)
{
	return m_bDirty = m_uData->SetString(MAX_TICK_TIME_MS, Strings::ToString(maxTickTimeMS));
}

bool GameConfig::SetTitleBarText(String text)
{
	return m_bDirty = m_uData->SetString(TITLEBAR_TEXT_KEY, std::move(text));
}

bool GameConfig::SetLogLevel(LogSeverity level)
{
	return m_bDirty = m_uData->SetString(LOG_LEVEL_KEY, severityMap[level]);
}

bool GameConfig::SetColliderBorderWidth(u32 shapeWidth)
{
	return m_bDirty = m_uData->SetString(COLLIDER_SHAPE_WIDTH_KEY, Strings::ToString(shapeWidth));
}

bool GameConfig::SetViewSize(Vector2 size)
{
	GData gData;
	gData.SetString("x", size.x.ToString());
	gData.SetString("y", size.y.ToString());
	return m_bDirty = m_uData->AddField(VIEW_SIZE_KEY, gData);
}

bool GameConfig::SetBackupLogFileCount(u8 count)
{
	return m_bDirty = m_uData->SetString(BACKUP_LOG_FILE_COUNT_KEY, Strings::ToString(count));
}

void GameConfig::Verify()
{
	m_bDirty = false;
	u32 backupLogFileCount = 5;
#if DEBUGGING
	backupLogFileCount = 3;
#endif
	m_bDirty |= SetStringIfEmpty(*m_uData, TITLEBAR_TEXT_KEY, "Async Little Engine");
	m_bDirty |= SetStringIfEmpty(*m_uData, LOG_LEVEL_KEY, "Info");
	m_bDirty |= SetStringIfEmpty(*m_uData, COLLIDER_SHAPE_WIDTH_KEY, "2");
	m_bDirty |= SetStringIfEmpty(*m_uData, TICKS_PER_SECOND_KEY, Strings::ToString(45));
	m_bDirty |= SetStringIfEmpty(*m_uData, RENDER_THREAD_START_DELAY_MS_KEY, Strings::ToString(10));
	m_bDirty |= SetStringIfEmpty(*m_uData, MAX_TICK_TIME_MS, Strings::ToString(100));
	m_bDirty |= SetStringIfEmpty(*m_uData, RENDER_THREAD_KEY, Strings::ToString(true));
	m_bDirty |= SetStringIfEmpty(*m_uData, JOB_WORKER_COUNT_KEY, Strings::ToString(4));
	m_bDirty |= SetStringIfEmpty(*m_uData, PAUSE_ON_FOCUS_LOSS_KEY, Strings::ToString(s_bPauseOnFocusLoss));
	m_bDirty |= SetStringIfEmpty(*m_uData, BACKUP_LOG_FILE_COUNT_KEY, Strings::ToString(backupLogFileCount));
	if (m_uData->GetString(VIEW_SIZE_KEY).empty())
	{
		SetViewSize({1920, 1080});
		m_bDirty = true;
	}
}
} // namespace LittleEngine
