#include "GameConfig.h"

using GData = Core::GData;

namespace LittleEngine
{
namespace
{
const std::string RENDER_THREAD_KEY = "bRenderThread";
const std::string JOB_WORKER_COUNT_KEY = "jobWorkerCount";
const std::string STEPS_PER_SECOND_KEY = "stepsPerSecond";
const std::string TICKS_PER_SECOND_KEY = "ticksPerSecond";
const std::string RENDER_THREAD_START_DELAY_MS_KEY = "renderThreadStartDelayMS";
const std::string MAX_FRAME_TIME_MS = "maxFrameTimeMS";
const std::string TITLEBAR_TEXT_KEY = "titleBarText";
const std::string UI_SPACE_KEY = "uiSpace";
const std::string WORLD_HEIGHT_KEY = "worldHeight";
const std::string COLLIDER_SHAPE_WIDTH_KEY = "colliderShapeBorderWidth";
const std::string BACKUP_LOG_FILE_COUNT_KEY = "backupLogFileCount";
const std::string ENTITY_ORIENTATION_SIZE_KEY = "entityOrientationSize";
const std::string CONTROLLER_ORIENTATION_SIZE_KEY = "controllerOrientationSize";
const std::string CONTROLLER_ORIENTATION_EPSILON_KEY = "controllerOrientationEpsilon";
const std::string FORCE_VIEWPORT_ASPECT_KEY = "forceViewportAR";

bool SetStringIfEmpty(GData& data, const std::string& key, std::string value)
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

GameConfig& GameConfig::Instance()
{
	static GameConfig s_config;
	if (!s_config.m_bInit)
	{
		s_config.Init();
	}
	return s_config;
}

void GameConfig::Init()
{
	Verify();
	m_bInit = true;
}

bool GameConfig::Load(std::string path)
{
	m_bDirty = true;
	FileRW file(std::move(path));
	bool bSuccess = m_data.Marshall(file.Load(true));
	Verify();
	return bSuccess;
}

bool GameConfig::Save(std::string path)
{
	FileRW file(std::move(path));
	if (m_bDirty || !file.Exists())
	{
		return file.Write(m_data.Unmarshall());
	}
	return true;
}

bool GameConfig::ShouldCreateRenderThread() const
{
	return m_data.GetBool(RENDER_THREAD_KEY);
}

u32 GameConfig::JobWorkerCount() const
{
	return static_cast<u32>(m_data.GetS32(JOB_WORKER_COUNT_KEY));
}

Time GameConfig::StepRate() const
{
	u32 ticks = static_cast<u32>(m_data.GetS32(STEPS_PER_SECOND_KEY));
	return Time::Seconds(1.0f / ticks);
}

Time GameConfig::TickRate() const
{
	u32 ticks = static_cast<u32>(m_data.GetS32(TICKS_PER_SECOND_KEY));
	return Time::Seconds(1.0f / ticks);
}

Time GameConfig::RenderThreadStartDelay() const
{
	return Time::Milliseconds(ToS32(m_data.GetS32(RENDER_THREAD_START_DELAY_MS_KEY)));
}

Time GameConfig::MaxFrameTime() const
{
	return Time::Milliseconds(ToS32(m_data.GetS32(MAX_FRAME_TIME_MS)));
}

std::string GameConfig::TitleBarText() const
{
	return m_data.GetString(TITLEBAR_TEXT_KEY);
}

u16 GameConfig::ColliderBorderWidth() const
{
	return static_cast<u16>(m_data.GetS32(COLLIDER_SHAPE_WIDTH_KEY));
}

Vector2 GameConfig::UISpace() const
{
	return m_data.GetVector2(UI_SPACE_KEY);
}

Fixed GameConfig::WorldHeight() const
{
	return Fixed(m_data.GetF64(WORLD_HEIGHT_KEY));
}

u8 GameConfig::BackupLogFileCount() const
{
	return static_cast<u8>(m_data.GetS32(BACKUP_LOG_FILE_COUNT_KEY));
}

Vector2 GameConfig::EntityOrientationSize() const
{
	return m_data.GetVector2(ENTITY_ORIENTATION_SIZE_KEY);
}

Vector2 GameConfig::ControllerOrientationSize() const
{
	return m_data.GetVector2(CONTROLLER_ORIENTATION_SIZE_KEY);
}

Fixed GameConfig::ControllerOrientationEpsilon() const
{
	return Fixed(m_data.GetF64(CONTROLLER_ORIENTATION_EPSILON_KEY));
}

f64 GameConfig::ForceViewportAR() const
{
	return m_data.GetF64(FORCE_VIEWPORT_ASPECT_KEY);
}

void GameConfig::Verify()
{
	m_bDirty = false;
	m_bDirty |= SetStringIfEmpty(m_data, TITLEBAR_TEXT_KEY, "LOC_VIEWPORT_TITLE");
	m_bDirty |= SetStringIfEmpty(m_data, COLLIDER_SHAPE_WIDTH_KEY, "2");
	m_bDirty |= SetStringIfEmpty(m_data, STEPS_PER_SECOND_KEY, Strings::ToString(100));
	m_bDirty |= SetStringIfEmpty(m_data, TICKS_PER_SECOND_KEY, Strings::ToString(35));
	m_bDirty |= SetStringIfEmpty(m_data, RENDER_THREAD_START_DELAY_MS_KEY, Strings::ToString(3));
	m_bDirty |= SetStringIfEmpty(m_data, MAX_FRAME_TIME_MS, Strings::ToString(50));
	m_bDirty |= SetStringIfEmpty(m_data, RENDER_THREAD_KEY, Strings::ToString(true));
	m_bDirty |= SetStringIfEmpty(m_data, JOB_WORKER_COUNT_KEY, Strings::ToString(4));
	m_bDirty |= SetStringIfEmpty(m_data, BACKUP_LOG_FILE_COUNT_KEY, Strings::ToString(5));
	m_bDirty |= SetStringIfEmpty(m_data, UI_SPACE_KEY, "{x:1920,y:1080}");
	m_bDirty |= SetStringIfEmpty(m_data, WORLD_HEIGHT_KEY, "1080");
	m_bDirty |= SetStringIfEmpty(m_data, ENTITY_ORIENTATION_SIZE_KEY, "{x:100,y:2}");
	m_bDirty |= SetStringIfEmpty(m_data, CONTROLLER_ORIENTATION_SIZE_KEY, "{x:120,y:3}");
	m_bDirty |= SetStringIfEmpty(m_data, CONTROLLER_ORIENTATION_EPSILON_KEY, Strings::ToString(0.025));

	m_bRenderThread = ShouldCreateRenderThread();
}
} // namespace LittleEngine
