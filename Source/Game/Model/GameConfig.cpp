#include "Core/GData.h"
#include "Core/Utils.h"
#include "GameConfig.h"

using GData = Core::GData;

namespace LittleEngine
{
namespace
{
const char* RENDER_THREAD_KEY = "bRenderThread";
const char* PAUSE_ON_FOCUS_LOSS_KEY = "bPauseOnFocusLoss";
const char* JOB_WORKER_COUNT_KEY = "jobWorkerCount";
const char* TICKS_PER_SECOND_KEY = "ticksPerSecond";
const char* RENDER_THREAD_START_DELAY_MS_KEY = "renderThreadStartDelayMS";
const char* MAX_FRAME_TIME_MS = "maxFrameTimeMS";
const char* TITLEBAR_TEXT_KEY = "titleBarText";
const char* VIEW_SIZE_KEY = "uiViewSize";
const char* WORLD_HEIGHT_KEY = "worldViewHeight";
const char* COLLIDER_SHAPE_WIDTH_KEY = "colliderShapeBorderWidth";
const char* BACKUP_LOG_FILE_COUNT_KEY = "backupLogFileCount";
const char* ENTITY_ORIENTATION_SIZE_KEY = "entityOrientationSize";
const char* CONTROLLER_ORIENTATION_SIZE_KEY = "controllerOrientationSize";
const char* CONTROLLER_ORIENTATION_EPSILON_KEY = "controllerOrientationEpsilon";
const char* FORCE_VIEWPORT_ASPECT_KEY = "forceViewportAR";

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
	bool bSuccess = m_uData->Marshall(file.Load(true));
	Verify();
	return bSuccess;
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

u32 GameConfig::JobWorkerCount() const
{
	return static_cast<u32>(m_uData->GetS32(JOB_WORKER_COUNT_KEY));
}

Time GameConfig::TickRate() const
{
	u32 ticks = static_cast<u32>(m_uData->GetS32(TICKS_PER_SECOND_KEY));
	return Time::Seconds(1.0f / ticks);
}

Time GameConfig::RenderThreadStartDelay() const
{
	return Time::Milliseconds(static_cast<s32>(m_uData->GetS32(RENDER_THREAD_START_DELAY_MS_KEY)));
}

Time GameConfig::MaxFrameTime() const
{
	return Time::Milliseconds(static_cast<s32>(m_uData->GetS32(MAX_FRAME_TIME_MS)));
}

String GameConfig::TitleBarText() const
{
	return m_uData->GetString(TITLEBAR_TEXT_KEY);
}

u16 GameConfig::ColliderBorderWidth() const
{
	return static_cast<u16>(m_uData->GetS32(COLLIDER_SHAPE_WIDTH_KEY));
}

Vector2 GameConfig::UIViewSize() const
{
	return m_uData->GetVector2(VIEW_SIZE_KEY);
}

Fixed GameConfig::WorldViewHeight() const 
{
	return Fixed(m_uData->GetF64(WORLD_HEIGHT_KEY));
}

u8 GameConfig::BackupLogFileCount() const
{
	return static_cast<u8>(m_uData->GetS32(BACKUP_LOG_FILE_COUNT_KEY));
}

Vector2 GameConfig::EntityOrientationSize() const
{
	return m_uData->GetVector2(ENTITY_ORIENTATION_SIZE_KEY);
}

Vector2 GameConfig::ControllerOrientationSize() const
{
	return m_uData->GetVector2(CONTROLLER_ORIENTATION_SIZE_KEY);
}

Fixed GameConfig::ControllerOrientationEpsilon() const
{
	return Fixed(m_uData->GetF64(CONTROLLER_ORIENTATION_EPSILON_KEY));
}

f64 GameConfig::ForceViewportAR() const 
{
	return m_uData->GetF64(FORCE_VIEWPORT_ASPECT_KEY);
}

void GameConfig::Verify()
{
	m_bDirty = false;
	m_bDirty |= SetStringIfEmpty(*m_uData, TITLEBAR_TEXT_KEY, "LOC_VIEWPORT_TITLE");
	m_bDirty |= SetStringIfEmpty(*m_uData, COLLIDER_SHAPE_WIDTH_KEY, "2");
	m_bDirty |= SetStringIfEmpty(*m_uData, TICKS_PER_SECOND_KEY, Strings::ToString(35));
	m_bDirty |= SetStringIfEmpty(*m_uData, RENDER_THREAD_START_DELAY_MS_KEY, Strings::ToString(3));
	m_bDirty |= SetStringIfEmpty(*m_uData, MAX_FRAME_TIME_MS, Strings::ToString(50));
	m_bDirty |= SetStringIfEmpty(*m_uData, RENDER_THREAD_KEY, Strings::ToString(true));
	m_bDirty |= SetStringIfEmpty(*m_uData, JOB_WORKER_COUNT_KEY, Strings::ToString(4));
	m_bDirty |= SetStringIfEmpty(*m_uData, PAUSE_ON_FOCUS_LOSS_KEY, Strings::ToString(s_bPauseOnFocusLoss));
	m_bDirty |= SetStringIfEmpty(*m_uData, BACKUP_LOG_FILE_COUNT_KEY, Strings::ToString(5));
	m_bDirty |= SetStringIfEmpty(*m_uData, VIEW_SIZE_KEY, "{x:1920,y:1080}");
	m_bDirty |= SetStringIfEmpty(*m_uData, WORLD_HEIGHT_KEY, "1080");
	m_bDirty |= SetStringIfEmpty(*m_uData, ENTITY_ORIENTATION_SIZE_KEY, "{x:100,y:2}");
	m_bDirty |= SetStringIfEmpty(*m_uData, CONTROLLER_ORIENTATION_SIZE_KEY, "{x:120,y:3}");
	m_bDirty |= SetStringIfEmpty(*m_uData, CONTROLLER_ORIENTATION_EPSILON_KEY, Strings::ToString(0.025));
	
	m_bRenderThread = ShouldCreateRenderThread();
}
} // namespace LittleEngine
