#include "stdafx.h"
#include "EngineConfig.h"
#include "Utils.h"

namespace LittleEngine
{
namespace
{
const String RENDER_THREAD_KEY = "renderThread";
const String PAUSE_ON_FOCUS_LOSS_KEY = "bPauseOnFocusLoss";
const String NUM_GAME_THREADS_KEY = "numGameThreads";
const String TICKS_PER_SECOND_KEY = "ticksPerSecond";
const String MAX_TICK_TIME_MS = "maxTickTimeMS";
const String WINDOW_TITLE_KEY = "windowTitle";
const String LOG_LEVEL_KEY = "logLevel";
const String VIEW_SIZE_KEY = "viewSize";
const String COLLIDER_SHAPE_WIDTH_KEY = "colliderShapeBorderWidth";

UMap<Core::LogSeverity, String> severityMap = {{Core::LogSeverity::Error, "Error"},
											   {Core::LogSeverity::Warning, "Warning"},
											   {Core::LogSeverity::Info, "Info"},
											   {Core::LogSeverity::Debug, "Debug"},
											   {Core::LogSeverity::HOT, "HOT"}};

Core::LogSeverity ParseLogLevel(const String& str)
{
	for (const auto& severity : severityMap)
	{
		if (severity.second == str)
			return severity.first;
	}
	return Core::LogSeverity::Info;
}

void SetStringIfEmpty(GData& data, const String& key, const String& value)
{
	if (data.GetString(key, "NULL") == "NULL")
		data.SetString(key, value);
}
} // namespace

const bool EngineConfig::s_bPauseOnFocusLoss = false;
const Version EngineConfig::s_engineVersion = "0.1.3";

EngineConfig::EngineConfig()
{
	m_data.Clear();
	Verify();
}

bool EngineConfig::Load(const String& path)
{
	m_bDirty = true;
	FileRW file(path);
	m_data.Marshall(file.ReadAll(true));
	bool success = m_data.NumFields() > 0;
	Verify();
	return success;
}

bool EngineConfig::Save(const String& path)
{
	FileRW file(path);
	if (m_bDirty || !file.Exists())
	{
		return file.Write(m_data.Unmarshall());
	}
	return true;
}

bool EngineConfig::ShouldCreateRenderThread() const
{
	return m_data.GetBool(RENDER_THREAD_KEY);
}

bool EngineConfig::ShouldPauseOnFocusLoss() const
{
	return m_data.GetBool(PAUSE_ON_FOCUS_LOSS_KEY);
}

u32 EngineConfig::GetNumGameThreads() const
{
	return static_cast<u32>(m_data.GetS32(NUM_GAME_THREADS_KEY));
}

u32 EngineConfig::GetTicksPerSecond() const
{
	return static_cast<u32>(m_data.GetS32(TICKS_PER_SECOND_KEY));
}

u32 EngineConfig::GetMaxTickTimeMS() const
{
	return static_cast<u32>(m_data.GetS32(MAX_TICK_TIME_MS));
}

String EngineConfig::GetWindowTitle() const
{
	return m_data.GetString(WINDOW_TITLE_KEY, '_', "(Value unset in Engine Config GData file)");
}

u32 EngineConfig::GetColliderBorderWidth() const
{
	return static_cast<u32>(m_data.GetS32(COLLIDER_SHAPE_WIDTH_KEY));
}

Core::LogSeverity EngineConfig::GetLogLevel() const
{
	return ParseLogLevel(m_data.GetString(LOG_LEVEL_KEY));
}

Vector2 EngineConfig::GetViewSize() const
{
	GData vec2 = m_data.GetGData(VIEW_SIZE_KEY);
	return Vector2(Fixed(vec2.GetF64("x")), Fixed(vec2.GetF64("y")));
}

const Version& EngineConfig::GetEngineVersion()
{
	return s_engineVersion;
}

bool EngineConfig::SetCreateRenderThread(bool bCreate)
{
	return m_bDirty = m_data.SetString(RENDER_THREAD_KEY, Strings::ToString(bCreate));
}

bool EngineConfig::SetPauseOnFocusLoss(bool bPause)
{
	return m_bDirty = m_data.SetString(PAUSE_ON_FOCUS_LOSS_KEY, Strings::ToString(bPause));
}

bool EngineConfig::SetNumGameThreads(u32 numThreads)
{
	return m_bDirty = m_data.SetString(NUM_GAME_THREADS_KEY, Strings::ToString(numThreads));
	;
}

bool EngineConfig::SetTicksPerSecond(u32 ticksPerSecond)
{
	return m_bDirty = m_data.SetString(TICKS_PER_SECOND_KEY, Strings::ToString(ticksPerSecond));
}

bool EngineConfig::SetMaxTimeMS(u32 maxTickTimeMS)
{
	return m_bDirty = m_data.SetString(MAX_TICK_TIME_MS, Strings::ToString(maxTickTimeMS));
}

bool EngineConfig::SetWindowTitle(const String& windowTitle)
{
	return m_bDirty = m_data.SetString(WINDOW_TITLE_KEY, windowTitle);
}

bool EngineConfig::SetLogLevel(LogSeverity level)
{
	return m_bDirty = m_data.SetString(LOG_LEVEL_KEY, severityMap[level]);
}

bool EngineConfig::SetColliderBorderWidth(u32 shapeWidth)
{
	return m_bDirty = m_data.SetString(COLLIDER_SHAPE_WIDTH_KEY, Strings::ToString(shapeWidth));
}

bool EngineConfig::SetViewSize(const Vector2& viewSize)
{
	GData gData;
	gData.SetString("x", viewSize.x.ToString());
	gData.SetString("y", viewSize.y.ToString());
	return m_bDirty = m_data.AddField(VIEW_SIZE_KEY, gData);
}

void EngineConfig::Verify()
{
	SetStringIfEmpty(m_data, WINDOW_TITLE_KEY, "Async Little Engine");
	SetStringIfEmpty(m_data, LOG_LEVEL_KEY, "Info");
	SetStringIfEmpty(m_data, COLLIDER_SHAPE_WIDTH_KEY, "1.0");
	SetStringIfEmpty(m_data, TICKS_PER_SECOND_KEY, Strings::ToString(40));
	SetStringIfEmpty(m_data, MAX_TICK_TIME_MS, Strings::ToString(25));
	SetStringIfEmpty(m_data, RENDER_THREAD_KEY, Strings::ToString(true));
	SetStringIfEmpty(m_data, NUM_GAME_THREADS_KEY, Strings::ToString(6));
	SetStringIfEmpty(m_data, PAUSE_ON_FOCUS_LOSS_KEY, Strings::ToString(s_bPauseOnFocusLoss));
	if (m_data.GetString(VIEW_SIZE_KEY).empty())
		SetViewSize(Vector2(1920, 1080));
	m_bDirty = false;
}
} // namespace LittleEngine
